// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Position.hpp"
#include "Stone.hpp"
#include "Utilities.hpp"

#include <array>
#include <cstdint>
#include <sstream>

#include "Error.hpp"


class GameState;


class Action {
    friend struct std::hash<Action>;

public:
    /// The maximum number of actions in a sequence per move.
    ///
    constexpr static uint8_t maximumPerMove = 2;

    enum Type : uint8_t {
        None = 0, ///< No action as indicator.
        PlaceStone, ///< Place a stone from the action pool on the board.
        ReplaceStone, ///< Replace a stone on the board with a stone from the action pool, and drop 1 stone.
        RotateStone, ///< Rotate 1 stone on the board, and drop 1 stone.
        DrawStone, ///< Draw 1 extra stone from the resource pool.
    };

    struct Data {
        uint8_t type : 4 = 0;
        uint8_t actionStone : 4 = 0;
        uint8_t droppedStone : 4 = 0;
        uint8_t orientation : 4 = 0;
    };

public:
    Action() = default;
    constexpr Action(
        const Type type,
        const Stone actionStone,
        const Stone droppedStone,
        const Orientation orientation,
        const Position position) noexcept
    :
        _data{
            .type = type,
            .actionStone = actionStone.type(),
            .droppedStone = droppedStone.type(),
            .orientation = orientation},
        _position{position} {
    }

public: // operators
    auto operator==(const Action &other) const noexcept -> bool {
        return _data.type == other._data.type &&
            _data.actionStone == other._data.actionStone &&
            _data.droppedStone == other._data.droppedStone &&
            _data.orientation == other._data.orientation &&
            _position == other._position;
    }
    auto operator!=(const Action &other) const noexcept -> bool { return not (*this == other); }

public: // attributes
    [[nodiscard]] constexpr auto isNone() const noexcept -> bool { return _data.type == None; }
    [[nodiscard]] auto type() const noexcept -> Type { return static_cast<Type>(_data.type); }
    [[nodiscard]] auto actionStone() const noexcept -> Stone { return Stone{_data.actionStone}; }
    [[nodiscard]] auto droppedStone() const noexcept -> Stone { return Stone{_data.droppedStone}; }
    [[nodiscard]] auto orientation() const noexcept -> Orientation { return static_cast<Orientation::Value>(_data.orientation); }
    [[nodiscard]] auto position() const noexcept -> Position { return _position; }

    [[nodiscard]] auto toString() const noexcept -> std::string {
        if (type() == None) {
            return {};
        }
        std::stringstream result;
        result << "Action(" << typeToString(type());
        switch (type()) {
        case PlaceStone:
            result << ", " << actionStone().toString() << ":" << orientation().toArrow()
                << " => " << _position.toString();
            break;
        case ReplaceStone:
            result << ", " << actionStone().toString() << ":" << orientation().toArrow()
                << " => " << _position.toString() << ", drop=" << droppedStone().toString();
            break;
        case RotateStone:
            result << ", " << _position.toString() << " by " << orientation().toArrow()
                << ", drop=" << droppedStone().toString();
            break;
        case DrawStone:
            result << ", " << actionStone().toString();
            break;
        default:
        case None:
            break;
        }
        result << ")";
        return result.str();
    }

    /// Apply this action to the given state.
    ///
    /// @throws Error in case this action is not valid for this state.
    ///
    void applyTo(GameState &state) const;

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return 1U + Stone::dataSize() + Orientation::dataSize() + Stone::dataSize() + Position::dataSize();
    }

    void addToData(std::string &data) const noexcept {
        data += utility::valueToHexDigit(type());
        data.append(actionStone().toString());
        data.append(orientation().toString());
        data.append(droppedStone().toString());
        position().addToData(data);
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> Action {
        if (data.size() != dataSize()) {
            throw Error("Action: Invalid data size.");
        }
        const auto type = static_cast<Type>(utility::hexDigitToValue(data[0]));
        const auto actionStone = Stone::fromData(data.substr(1U, Stone::dataSize()));
        const auto orientation = Orientation::fromData(data.substr(1 + Stone::dataSize(), Orientation::dataSize()));
        const auto droppedStone = Stone::fromData(data.substr(1U + Stone::dataSize() + Orientation::dataSize(), Stone::dataSize()));
        const auto position = Position::fromData(data.substr(1U + Stone::dataSize() + Orientation::dataSize() + Stone::dataSize()));
        return Action{type, actionStone, droppedStone, orientation, position};
    }

public:
    static constexpr auto types() noexcept -> std::array<Type, 4> {
        return {PlaceStone, ReplaceStone, RotateStone, DrawStone};
    }

    [[nodiscard]] static auto typeToString(const Type type) noexcept -> std::string {
        switch (type) {
        case PlaceStone: return "Place";
        case ReplaceStone: return "Replace";
        case RotateStone: return "Rotate";
        case DrawStone: return "Draw";
        default: return {};
        }
    }

public:
    constexpr static auto createRotate(const Position pos, const Orientation newOrientation, const Stone droppedStone) -> Action {
        return Action{RotateStone, {}, droppedStone, newOrientation, pos};
    }
    constexpr static auto createReplace(const Position pos, const Stone stone, const Orientation orientation, const Stone droppedStone) -> Action {
        return Action{ReplaceStone, stone, droppedStone, orientation, pos};
    }
    constexpr static auto createPlace(const Position pos, const Stone stone, const Orientation orientation) -> Action {
        return Action{PlaceStone, stone, {}, orientation, pos};
    }
    constexpr static auto createDraw(const Stone stone) noexcept -> Action {
        return Action{DrawStone, stone, {}, {}, Position::invalid()};
    }

private:
    void applyPlaceAction(GameState &state) const;
    void applyReplaceAction(GameState &state) const;
    void applyRotateAction(GameState &state) const;
    void applyDrawAction(GameState &state) const;

private:
    Data _data; ///< The bitfield with the action values.
    Position _position = Position::invalid(); ///< The position where the stone is placed, replaced or rotated.
};


template<>
struct std::hash<Action> {
    auto operator()(const Action &gameAction) const noexcept -> std::size_t {
        return utility::hashFromValues(
            gameAction.type(),
            gameAction.actionStone(),
            gameAction.droppedStone(),
            gameAction.orientation(),
            gameAction.position());
    }
};

