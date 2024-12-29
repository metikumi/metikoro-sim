// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ActionSequence.hpp"
#include "OrbMove.hpp"

#include <sstream>


class GameMove {
    friend struct std::hash<GameMove>;

public:
    GameMove() = default;
    GameMove(const ActionSequence &actions, const Stone drawnStone, const OrbMove orbMove) noexcept
        : _actions{actions}, _drawnStone{drawnStone}, _orbMove{orbMove} {
    }

public: // operators
    auto operator==(const GameMove &other) const noexcept -> bool = default;

public: // attributes
    [[nodiscard]] auto isNoMove() const noexcept -> bool { return _actions.hasNoActions(); }
    [[nodiscard]] auto actions() const noexcept -> const ActionSequence& { return _actions; }
    [[nodiscard]] auto drawnStone() const noexcept -> Stone { return _drawnStone; }
    [[nodiscard]] auto orbMove() const noexcept -> const OrbMove& { return _orbMove; }

public: // serialization
    [[nodiscard]] auto toData() const noexcept -> std::string {
        std::string result;
        result.reserve(dataSize());
        addToData(result);
        return result;
    }

    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return 3U + ActionSequence::dataSize() + Stone::dataSize() + OrbMove::dataSize();
    }

    void addToData(std::string &data) const noexcept {
        data.append("M1:");
        _actions.addToData(data);
        _drawnStone.addToData(data);
        _orbMove.addToData(data);
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> GameMove {
        if (data.size() != dataSize()) {
            throw Error("GameMove: Invalid data size.");
        }
        if (data.substr(0, 3U) != "M1:") {
            throw Error("GameMove: Invalid data prefix.");
        }
        const auto actions = ActionSequence::fromData(data.substr(3U, ActionSequence::dataSize()));
        const auto drawnStone = Stone::fromData(data.substr(3U + ActionSequence::dataSize(), Stone::dataSize()));
        const auto orbMove = OrbMove::fromData(data.substr(3U + ActionSequence::dataSize() + Stone::dataSize(), OrbMove::dataSize()));
        return GameMove{actions, drawnStone, orbMove};
    }

public: // conversion
    [[nodiscard]] auto toString() const noexcept -> std::string {
        std::stringstream result;
        result << "GameMove(";
        result << _actions.toString();
        result << ", Draw(";
        if (not _drawnStone.empty()) {
            result << _drawnStone.toString();
        }
        result << "), OrbMove(" << _orbMove.toString() << "))";
        return result.str();
    }

private:
    ActionSequence _actions; ///< The action sequence.
    Stone _drawnStone; ///< The regular drawn stone, or Empty if no one is left in the pool.
    OrbMove _orbMove; ///< The optional orb move.
};


static_assert(Serializable<GameMove>);


using GameMoves = std::vector<GameMove>;


template<>
struct std::hash<GameMove> {
    auto operator()(const GameMove &gameMove) const noexcept -> std::size_t {
        return utility::hashFromValues(gameMove._actions, gameMove._drawnStone, gameMove._orbMove);
    }
};

