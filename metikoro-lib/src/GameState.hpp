// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ActionPools.hpp"
#include "ActionSequences.hpp"
#include "Board.hpp"
#include "Error.hpp"
#include "GameMove.hpp"
#include "GridOutput.hpp"
#include "OrbMoves.hpp"
#include "OrbPositions.hpp"
#include "Player.hpp"
#include "ResourcePool.hpp"

#include <functional>


class GameState {
    friend struct std::hash<GameState>;
    using MoveAddFn = std::function<void(const GameMove&)>;

public:
    GameState() = default;

public:
    [[nodiscard]] auto operator==(const GameState &other) const -> bool {
        return _board == other._board && _actionPools == other._actionPools &&
            _orbPositions == other._orbPositions && _resourcePool == other._resourcePool;
    }
    [[nodiscard]] auto operator!=(const GameState &other) const -> bool { return not (*this == other); }

public: // attributes
    [[nodiscard]] auto board() const noexcept -> const Board& { return _board; }
    [[nodiscard]] auto board() noexcept -> Board& { return _board; }
    [[nodiscard]] auto actionPools() const noexcept -> const ActionPools& { return _actionPools; }
    [[nodiscard]] auto actionPools() noexcept -> ActionPools& { return _actionPools; }
    [[nodiscard]] auto orbPositions() const noexcept -> const OrbPositions& { return _orbPositions; }
    [[nodiscard]] auto orbPositions() noexcept -> OrbPositions& { return _orbPositions; }
    [[nodiscard]] auto resourcePool() const noexcept -> const ResourcePool& { return _resourcePool; }
    [[nodiscard]] auto resourcePool() noexcept -> ResourcePool& { return _resourcePool; }

public:
    [[nodiscard]] auto hasWinner() const noexcept -> bool {
        const auto orbsInHouse = this->orbsInHouse();
        return std::ranges::find_if(orbsInHouse, [](uint8_t count) {
            return count >= setup::orbCountToWin;
        }) != orbsInHouse.end();
    }

    [[nodiscard]] auto winningPlayer() const -> std::optional<Player> {
        const auto orbsInHouse = this->orbsInHouse();
        for (auto player : Player::all()) {
            if (orbsInHouse[player] >= setup::orbCountToWin) {
                return player;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] auto orbsInHouse() const noexcept -> std::array<uint8_t, Player::count> {
        std::array<uint8_t, Player::count> result = {0, 0, 0, 0};
        for (auto player : Player::all()) {
            for (auto pos : _board.houseOrbPositions(player)) {
                if (_orbPositions.isOrbAt(pos)) {
                    ++result.at(player);
                }
            }
        }
        return result;
    }

    void executeMove(const GameMove &move) {
        nextTurn();
        move.actions().applyTo(*this);
        if (not move.drawnStone().empty()) {
            moveStoneToPlayer(move.drawnStone(), Player{0});
        }
        if (not move.orbMove().isNoMove()) {
            move.orbMove().applyTo(*this);
        }
    }

    [[nodiscard]] auto afterAction(const ActionSequence &actionSequence) const noexcept -> GameState {
        auto temporaryState = *this;
        actionSequence.applyTo(temporaryState);
        return temporaryState;
    }

    [[nodiscard]] auto afterMove(const GameMove &move) const noexcept -> GameState {
        auto temporaryState = *this;
        temporaryState.executeMove(move);
        return temporaryState;
    }

    /// Prepare the state for the next turn.
    ///
    /// This will decrease the Ko lock counter and unlock certain moves.
    ///
    void nextTurn() {
        _board.nextTurn();
        _orbPositions.nextTurn();
    }

    /// Create a rotated version of this board.
    ///
    [[nodiscard]] auto rotated(const Rotation rotation) const noexcept -> GameState {
        return GameState{
            _board.rotated(rotation),
            _actionPools.rotated(rotation),
            _orbPositions.rotated(rotation),
            _resourcePool,
        };
    }

    /// Rotate the state into its original position for a given player.
    ///
    /// @param player The actual player that plays the top left position.
    /// @return The state, rotated that this corner is at the original player position.
    ///
    [[nodiscard]] auto rotatedForPlayer(const Player player) const noexcept -> GameState {
        switch (player) {
        case Player{1}:
            return rotated(Rotation::Clockwise270);
        case Player{2}:
            return rotated(Rotation::Clockwise180);
        case Player{3}:
            return rotated(Rotation::Clockwise90);
        default:
            return *this;
        }
    }

    /// Generate all possible moves for this state.
    ///
    [[nodiscard]] auto allMoves() const noexcept -> GameMoves {
        GameMoves moves;
        auto actionSequences = allActions().actions();
        for (const auto &actionSeq : actionSequences) {
            auto stateAfterAction = afterAction(actionSeq);
            for (const auto drawStone : stateAfterAction.allRegularDraws()) {
                for (const auto orbMove : stateAfterAction.allOrbMoves()) {
                    moves.emplace_back(actionSeq, drawStone, orbMove);
                }
            }
        }
        return moves;
    }

    /// Get all possible action sequences from the current state of the game.
    ///
    [[nodiscard]] auto allActions() const noexcept -> ActionSequences {
        return ActionSequences::allForState(*this);
    }

    /// Get all possible regular draws for the current state.
    ///
    /// Make sure you apply the chosen action(s), before you call this method.
    ///
    [[nodiscard]] auto allRegularDraws() const noexcept -> StoneList {
        if (actionPools().active().full()) {
            return {};
        }
        return _resourcePool.allRegularDraws();
    }

    /// Get all possible orb movements for the current state.
    ///
    /// Make sure you apply the chose action(s), before calling this method.
    ///
    [[nodiscard]] auto allOrbMoves() const noexcept -> OrbMoves {
        return OrbMoves::allForState(*this);
    }

private:
    void moveStoneToPlayer(const Stone stone, const Player player) {
        if (not _resourcePool.hasStone(stone)) {
            throw Error("Not enough stones in resource pool to move one to the player.");
        }
        if (_actionPools[player].full()) {
            throw Error("The action board was full when trying to move a stone from the resource pool.");
        }
        _resourcePool.take(stone);
        _actionPools[player].add(stone);
    }

public:
    static auto createStartingGameState() -> GameState {
        GameState gameState{};
        // Add all game stones to the resource pool.
        for (auto [count, stone] : setup::resourcePoolStones) {
            gameState._resourcePool.add(stone, count);
        }
        // Prepare the stone pools.
        for (auto player : Player::all()) {
            for (auto [count, stone] : setup::actionPoolStones) {
                for (auto i = 0; i < count; ++i) {
                    gameState.moveStoneToPlayer(stone, player);
                }
            }
        }
        // Place the initial orbs to the source fields.
        for (const auto pos : Board::sourceOrbPositions()) {
            gameState._orbPositions.moveOrb(Position::invalid(), pos);
        }
        return gameState;
    }

public: // serialization
    [[nodiscard]] auto toData() const noexcept -> std::string {
        std::string data;
        data.reserve(dataSize());
        addToData(data);
        return data;
    }

    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return 3U + Board::dataSize() + ActionPools::dataSize() + OrbPositions::dataSize() + ResourcePool::dataSize();
    }

    void addToData(std::string &data) const noexcept {
        data.append("S1:");
        _board.addToData(data);
        _actionPools.addToData(data);
        _orbPositions.addToData(data);
        _resourcePool.addToData(data);
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> GameState {
        if (data.size() != dataSize()) {
            throw Error("GameState: Invalid data size.");
        }
        if (data.substr(0U, 3U) != "S1:") {
            throw Error("GameState: Invalid data prefix.");
        }
        auto board = Board::fromData(data.substr(3U, Board::dataSize()));
        auto actionPools = ActionPools::fromData(data.substr(3U + Board::dataSize(), ActionPools::dataSize()));
        auto orbPositions = OrbPositions::fromData(data.substr(3U + Board::dataSize() + ActionPools::dataSize(), OrbPositions::dataSize()));
        auto resourcePool = ResourcePool::fromData(data.substr(3U + Board::dataSize() + ActionPools::dataSize() + OrbPositions::dataSize(), ResourcePool::dataSize()));
        return GameState{board, actionPools, orbPositions, resourcePool};
    }

public: // display
    [[nodiscard]] auto toString() const noexcept -> std::string {
        auto poolLines = actionPoolsToLines();
        auto boardLines = boardToLines();
        auto ressourcePoolLines = poolToLines();
        auto orbsLines = orbsToLines();
        return StringLines::fromColumns({
            poolLines, boardLines, ressourcePoolLines, orbsLines}, 1).toString();
    }

    [[nodiscard]] auto boardToLines() const noexcept -> StringLines {
        StringLines lines;
        lines.append("Board:");
        auto grid = GridOutput{GridOutput::GridBoldBorder, 5, setup::boardSize, setup::boardSize};
        lines.append(grid.toLines([this](uint8_t x, uint8_t y) {
            const auto pos = Position{x, y};
            return _board.toString(pos, _orbPositions.isOrbAt(pos));
        }));
        lines.extendLines();
        return lines;
    }

    [[nodiscard]] auto actionPoolsToLines() const noexcept -> StringLines {
        StringLines lines;
        for (auto player : Player::all()) {
            lines.append(std::format("Player {}:", static_cast<int>(player)));
            auto grid = GridOutput{GridOutput::BorderOnly, 3, 3, 2};
            lines.append(grid.toLines([this, player](uint8_t x, uint8_t y) -> std::string {
                const auto index = static_cast<uint8_t>(x + y * 3U);
                return std::string{" "} + _actionPools[player].at(index).toString() + ' ';
            }));
        }
        lines.extendLines();
        return lines;
    }

    [[nodiscard]] auto poolToLines() const noexcept -> StringLines {
        StringLines lines;
        lines.append("R-Pool:");
        auto grid = GridOutput{GridOutput::GridVertical, 2, 2, Stone::count - 1};
        lines.append(grid.toLines([this](uint8_t x, uint8_t y) -> std::string {
            const auto stone = Stone{static_cast<Stone::Type>(y + 1)};
            if (x == 0) {
                return std::format("{:2}", stone.toString());
            }
            return std::format("{:2}", _resourcePool.count(stone));
        }));
        lines.extendLines();
        return lines;
    }

    [[nodiscard]] auto orbsToLines() const noexcept -> StringLines {
        StringLines lines;
        lines.append("Orbs:");
        const auto grid = GridOutput{GridOutput::GridVertical, 3, 3, setup::orbCount};
        lines.append(grid.toLines([this](uint8_t x, uint8_t y) -> std::string {
            auto orbPosition = orbPositions().positions().at(y);
            if (orbPosition.position.isInvalid()) {
                return " - ";
            }
            if (x == 0) {
                return std::format("{:3}", orbPosition.position.toString(false));
            }
            if (x == 1) {
                if (orbPosition.koPosition.isInvalid()) {
                    return " - ";
                }
                return std::format("{:3}", orbPosition.koPosition.toString(false));
            }
            if (orbPosition.koPosition.isInvalid()) {
                return "   ";
            }
            return std::format(" {:2}", orbPosition.koLock);
        }));
        return lines;
    }

private:
    GameState(
        const Board &board,
        const ActionPools &actionPools,
        const OrbPositions &orbPositions,
        const ResourcePool &resourcePool)
    :
        _board{board}, _actionPools{actionPools}, _orbPositions{orbPositions}, _resourcePool{resourcePool} {
    }

private:
    Board _board;
    ActionPools _actionPools;
    OrbPositions _orbPositions;
    ResourcePool _resourcePool;
};


static_assert(Serializable<GameState>);


template<>
struct std::hash<GameState> {
    auto operator()(const GameState &gameState) const noexcept -> std::size_t {
        return utility::hashFromValues(gameState._board, gameState._actionPools, gameState._orbPositions, gameState._resourcePool);
    }
};
