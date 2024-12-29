// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "GameLog.hpp"
#include "GameResult.hpp"
#include "Player.hpp"
#include "Agent.hpp"

#include <unordered_set>



class GameSimulator {
public:
    using ProgressFn = std::function<void(Player, const GameState&, const GameLog&, GameResult, std::size_t)>;

public:
    explicit GameSimulator(PlayerAgents agents) : _agents{std::move(agents)} {};

public:
    /// Run the simulation
    ///
    /// @return The final state, rotated to match the original player arrangement.
    ///
    auto run() -> GameState {
        _state = GameState::createStartingGameState();
        _currentPlayer = Player{0};
        std::size_t loopCount = 0;
        std::size_t turnCount = 0;
        while (not _state.hasWinner() and loopCount < setup::loopCountForDraw) {
            auto nextMove = _agents.at(_currentPlayer)->nextMove(_state, _gameLog);
            _gameLog.addTurn(turnCount, _currentPlayer, _state, nextMove);
            _state.executeMove(nextMove);
            turnCount += 1; // Just after executing the move, the turn ended and a new turn began.
            if (_state.hasWinner()) {
                if (_progressFn) {
                    _progressFn(_currentPlayer, _state, _gameLog, GameResult::Win, loopCount);
                }
                break;
            }
            if (_progressFn) {
                _progressFn(_currentPlayer, _state, _gameLog, GameResult::None, loopCount);
            }
            _state = _state.rotated(Rotation::Clockwise90);
            _currentPlayer.next();
            if (_states.contains(_state)) {
                if (++loopCount > setup::loopCountForDraw) {
                    if (_progressFn) {
                        _progressFn(_currentPlayer, _state, _gameLog, GameResult::Draw, loopCount);
                    }
                    break;
                }
            }
            _states.insert(_state);
        }
        _gameLog.addLastState(turnCount, _currentPlayer, _state);
        return _state.rotatedForPlayer(_currentPlayer);
    }

    /// Set a progress function.
    ///
    void setProgressFn(const ProgressFn& progressFn) {
        _progressFn = progressFn;
    }

    /// Access the complete game history.
    ///
    [[nodiscard]] auto gameLog() const -> const GameLog& {
        return _gameLog;
    }

private:
    GameState _state; ///< The current state.
    Player _currentPlayer; ///< The current player.
    PlayerAgents _agents; ///< The agents playing the game.
    GameLog _gameLog; ///< The game moves so far.
    std::unordered_set<GameState> _states; ///< Previously encountered game states.
    ProgressFn _progressFn{}; ///< A progress function to report the current progress of the simulation.
};
