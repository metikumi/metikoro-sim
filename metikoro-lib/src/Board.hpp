// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "BoardFrame.hpp"
#include "Error.hpp"
#include "Field.hpp"
#include "FieldGrid.hpp"
#include "GridOutput.hpp"
#include "Position.hpp"
#include "Setup.hpp"

#include <array>
#include <numeric>
#include <sstream>


//     P0             P3
//        FFFFF|FFFFF
//        F....|....F
//        F....|....F
//        F....|....F
//        F....|....F
//        -----+-----
//        F....|....F
//        F....|....F
//        F....|....F
//        F....|....F
//        FFFFF|FFFFF
//     P1             P2


class Board {
    friend struct std::hash<Board>;

public:
    using State = FieldGrid<Field, setup::boardSize - 2>;

public:
    Board() = default;

public:
    [[nodiscard]] auto operator==(const Board &other) const -> bool = default;

public:
    [[nodiscard]] auto field(Position position) const -> Field {
        if (const auto frameField = _frame.field(position); frameField.isStatic()) {
            return frameField.toField();
        }
        return stateField(position);
    }

    void setNewOrientation(const Position position, const Orientation newOrientation) {
        if (isStatic(position) or not stateField(position).canRotate()) {
            throw Error("Tried to rotate static field.");
        }
        stateField(position).setOrientation(newOrientation);
    }

    void setField(const Position position, const Stone stone, const Orientation orientation) {
        if (isStatic(position)) {
            throw Error("Tried to change static field.");
        }
        stateField(position).setStone(stone, orientation);
    }

    [[nodiscard]] auto rotated(Rotation rotation) const -> Board {
        return Board{_state.rotated(rotation)}; // frame is rotation symmetric.
    }

    [[nodiscard]] auto canPlayerPlaceStone(const Position position) const noexcept -> bool {
        return not isStatic(position) and field(position).empty() and not (
            isGarden(position) and playerForField(position) != Player{0});
    }

    [[nodiscard]] auto canPlayerReplaceStone(
        const Position position,
        const Stone newStone,
        const Orientation orientation) const noexcept -> bool {

        if (isStatic(position)) {
            return false;
        }
        return stateField(position).isValidChange(newStone, orientation);
    }

    [[nodiscard]] auto canPlayerRotateStone(const Position position, const Orientation newOrientation) const noexcept -> bool {
        if (isStatic(position)) {
            return false;
        }
        const auto field = stateField(position);
        return field.isValidChange(field.stone(), newOrientation);
    }

    [[nodiscard]] auto allPlaceOneActionPositions() const noexcept -> PositionList {
        PositionList result;
        std::size_t count = 0;
        for (const auto position : allNonStaticPositions()) {
            if (canPlayerPlaceStone(position)) {
                count += 1;
            }
        }
        result.reserve(count);
        for (auto position : allNonStaticPositions()) {
            if (canPlayerPlaceStone(position)) {
                result.push_back(position);
            }
        }
        return result;
    }

    [[nodiscard]] auto allPlaceTwoActionPositions() const noexcept -> PositionPairList {
        const auto positions = allPlaceOneActionPositions();
        return generateAllCombinedPositionPairs(positions);
    }

    [[nodiscard]] auto allReplaceOneActionPositions() const noexcept -> PositionList {
        return allNonStaticPositionsIf([](const Field& field) {
            return not field.empty() and not field.hasKoLock();
        });
    }

    [[nodiscard]] auto allReplaceTwoActionPositions() const noexcept -> PositionPairList {
        const auto positions = allReplaceOneActionPositions();
        return generateAllCombinedPositionPairs(positions);
    }

    [[nodiscard]] auto allRotateOneActionPositions() const noexcept -> PositionList {
        return allNonStaticPositionsIf([](const Field& field) { return not field.empty() && field.canRotate(); });
    }

    [[nodiscard]] auto allRotateTwoActionPositions() const noexcept -> PositionPairList {
        const auto positions = allRotateOneActionPositions();
        return generateAllCombinedPositionPairs(positions);
    }

    [[nodiscard]] auto houseOrbPositions(const Player player) const noexcept -> const BoardFrame::HouseOrbPositions& {
        return _frame.houseOrbPositions(player);
    }

    [[nodiscard]] static auto sourceOrbPositions() noexcept -> const BoardFrame::SourceOrbPositions& {
        return _frame.sourceOrbPositions();
    }

    void nextTurn() noexcept {
        for (const auto pos : allNonStaticPositions()) {
            stateField(pos).nextTurn();
        }
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return State::dataSize();
    }

    void addToData(std::string &data) const noexcept {
        _state.addToData(data);
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> Board {
        if (data.size() != dataSize()) {
            throw Error("Board: Invalid data size.");
        }
        return Board{State::fromData(data)};
    }

public: // display
    [[nodiscard]] auto toString() const noexcept -> std::string {
        constexpr auto grid = GridOutput{GridOutput::GridBoldBorder, 5, setup::boardSize, setup::boardSize};
        return grid.toLines([this](uint8_t x, uint8_t y) {
            const auto pos = Position{x, y};
            return toString(pos, false);
        }).toString();
    }

    [[nodiscard]] auto toString(const Position position, bool isOrbAtThisPosition) const noexcept -> std::string {
        std::string result;
        result.reserve(3);
        const auto area = _frame.field(position).area();
        result += area.toChar();
        if (area == BoardArea::House or area == BoardArea::Garden) {
            result += _frame.field(position).player().toChar();
        } else {
            result += ' ';
        }
        result += isOrbAtThisPosition ? "●" : " ";
        result.append(field(position).toBoardString());
        return result;
    }

    [[nodiscard]] auto toDebugString() const noexcept -> std::string {
        std::stringstream result;
        result << "State: ";
        std::size_t count = 0;
        for (const auto& field : _state.fields()) {
            result << count << ": " << field.toDebugString() << "\n";
            ++count;
        }
        return result.str();
    }

public:
    [[nodiscard]] static auto rotatedPosition(Rotation rotation, Position position) -> Position {
        return position.rotated(rotation, setup::boardSize);
    }

    [[nodiscard]] static auto isSource(const Position position) -> bool {
        return _frame.field(position).area() == BoardArea::Source;
    }

    [[nodiscard]] static auto isHouse(const Position position) -> bool {
        return _frame.field(position).area() == BoardArea::House;
    }

    [[nodiscard]] static auto isFrame(const Position position) -> bool {
        return _frame.field(position).area() == BoardArea::Frame;
    }

    [[nodiscard]] static auto isGarden(const Position position) -> bool {
        return _frame.field(position).area() == BoardArea::Garden;
    }

    [[nodiscard]] static auto playerForField(const Position position) -> Player {
        return _frame.field(position).player();
    }

    [[nodiscard]] static auto isStatic(const Position position) -> bool {
        return _frame.field(position).isStatic();
    }

private:
    explicit Board(const State &state) : _state{state} {};

    [[nodiscard]] auto stateField(const Position position) -> Field& {
        if (position.x() < 1 && position.x() > (State::sideLength - 1)) {
            throw Error("Board::stateField - X out of bounds");
        }
        if (position.y() < 1 && position.y() > (State::sideLength - 1)) {
            throw Error("Board::stateField - Y out of bounds");
        }
        return _state.field(position - Position{1, 1});
    }

    [[nodiscard]] auto stateField(const Position position) const -> Field {
        if (position.x() < 1 && position.x() > (State::sideLength - 1)) {
            throw Error("Board::stateField - X out of bounds");
        }
        if (position.y() < 1 && position.y() > (State::sideLength - 1)) {
            throw Error("Board::stateField - Y out of bounds");
        }
        return _state.field(position - Position{1, 1});
    }

    [[nodiscard]] static auto generateAllPositions() noexcept -> PositionList {
        PositionList result;
        result.reserve(setup::boardSize * setup::boardSize);
        for (Length y = 1; y < setup::boardSize - 1; ++y) {
            for (Length x = 1; x < setup::boardSize - 1; ++x) {
                result.emplace_back(x, y);
            }
        }
        return result;
    }

    [[nodiscard]] static auto allPositions() noexcept -> const PositionList& {
        static const PositionList positions = generateAllPositions();
        return positions;
    }

    [[nodiscard]] static auto allNonStaticPositions() noexcept -> const PositionList& {
        static const PositionList positions =
            generateAllPositions()
            | std::views::filter([](const Position& position) { return not isStatic(position); })
            | std::ranges::to<std::vector>();
        return positions;
    }

    [[nodiscard]] auto allNonStaticPositionsIf(const std::function<bool(const Field&)> &testFn) const noexcept -> PositionList {
        PositionList result;
        std::size_t count = 0;
        for (const auto position : allNonStaticPositions()) {
            if (testFn(stateField(position))) {
                count += 1;
            }
        }
        result.reserve(count);
        for (auto position : allNonStaticPositions()) {
            // if `stateField` directly, as this saves one necessary test.
            if (testFn(stateField(position))) {
                result.push_back(position);
            }
        }
        return result;
    }

    [[nodiscard]] static auto generateAllCombinedPositionPairs(const PositionList &positions) noexcept -> PositionPairList {
        if (positions.size() < 2) {
            return {}; // No pairs possible if fewer than 2 positions
        }
        PositionPairList result;
        result.reserve(positions.size() * (positions.size() - 1) / 2);
        for (auto it = positions.begin(); it != positions.end(); ++it) {
            for (auto jt = it + 1; jt != positions.end(); ++jt) {
                result.emplace_back(*it, *jt);
            }
        }
        return result;
    }

private:
    static inline BoardFrame _frame{};
    State _state{};
};


static_assert(Serializable<Board>);


template<>
struct std::hash<Board> {
    auto operator()(const Board &board) const noexcept -> std::size_t {
        return std::hash<Board::State>{}(board._state);
    }
};