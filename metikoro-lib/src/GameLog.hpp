// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "GameTurn.hpp"
#include "RatingAdjustment.hpp"

#include <list>



/// The log of a complete game.
///
class GameLog {
    constexpr static std::size_t usualMaxTurns = 256;

public:
    using GameTurns = std::vector<GameTurn>;

public:
    GameLog() = default;

public: // accessors
    [[nodiscard]] auto turns() const noexcept -> const GameTurns& {
        return _turns;
    }
    [[nodiscard]] auto size() const noexcept -> std::size_t {
        return _turns.size();
    }
    [[nodiscard]] auto empty() const noexcept -> bool {
        return _turns.empty();
    }
    [[nodiscard]] auto begin() const noexcept -> GameTurns::const_iterator {
        return _turns.begin();
    }
    [[nodiscard]] auto end() const noexcept -> GameTurns::const_iterator {
        return _turns.end();
    }

public: // modifiers
    void addTurn(const GameTurn &turn) noexcept {
        if (_turns.empty()) {
            _turns.reserve(usualMaxTurns);
        }
        _turns.emplace_back(turn);
    }
    void addTurn(
        const std::size_t turn,
        const Player player,
        const GameState &gameState,
        const GameMove &gameMove) noexcept {

        if (_turns.empty()) {
            _turns.reserve(usualMaxTurns);
        }
        _turns.emplace_back(turn, player, gameState, gameMove);
    }
    void addLastState(
        const std::size_t turn,
        const Player player,
        const GameState &gameState) noexcept {

        _turns.emplace_back(turn, player, gameState, GameMove{});
    }

public: // game analysis
    [[nodiscard]] auto winningPlayer() const -> std::optional<Player> {
        if (_turns.size() < 2) {
            return std::nullopt;
        }
        const auto &lastState = _turns.back().state;
        if (const auto winningPlayer = lastState.winningPlayer()) {
            const auto playerExecutedWinningMove = (_turns.end() - 2)->activePlayer;
            return playerExecutedWinningMove.offsetWith(winningPlayer.value());
        }
        return std::nullopt;
    }
    [[nodiscard]] auto createRatingAdjustments() const noexcept -> RatingAdjustments {
        const auto winningPlayer = this->winningPlayer();
        const auto totalTurnCount = size();
        RatingAdjustments result;
        result.reserve(_turns.size());
        std::ranges::transform(
            _turns,
            std::back_inserter(result),
            [winningPlayer, totalTurnCount](const GameTurn &turn) {
                return RatingAdjustment(turn, totalTurnCount, winningPlayer);
            }
        );
        return result;
    }

private:
    GameTurns _turns;
};

