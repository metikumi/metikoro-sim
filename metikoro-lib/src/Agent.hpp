// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ConsoleWriter.hpp"
#include "GameLog.hpp"
#include "Player.hpp"

#include <iostream>
#include <memory>
#include <span>
#include <string_view>
#include <vector>


class Agent;
using AgentPtr = std::shared_ptr<Agent>;


/// An agent playing the game.
///
class Agent : public ConsoleWriter, public std::enable_shared_from_this<Agent> {
public:
    ~Agent() override = default;

public:
    /// Display help.
    ///
    [[nodiscard]] static auto getHelp() noexcept -> std::string {
        return {}; // Implement this in the derived class to display help.
    }

    /// Initialize the agent.
    ///
    /// Called once for each agent after creating the storage object.
    ///
    /// @param args The arguments for this agent.
    /// @throws Error in case there was a problem with the command line arguments.
    ///
    virtual void initialize(std::span<std::string_view> args) = 0;

    /// Return a configuration string that is displayed at the start.
    ///
    virtual auto configurationString() const noexcept -> std::string {
        return {};
    }

    /// Create a copy of the agent for each thread.
    ///
    /// This method is called once in the main thread, *before* the threads are starting.
    /// If your agent is stateless, you can return its own instance.
    ///
    virtual auto copyForThread() noexcept -> AgentPtr = 0;

    /// Called before a new game starts.
    ///
    virtual void gameStart() = 0;

    /// Choose the next move for this state and game log.
    ///
    /// @warning This method *must be thread safe*! It is called from all simulation threads.
    ///
    /// @param state The current state, where this agent is player 0.
    /// @param gameLog The game log so far.
    ///
    [[nodiscard]] virtual auto nextMove(
        const GameState &state,
        const GameLog &gameLog) -> GameMove = 0;

    /// Called after a game ended.
    ///
    /// @param gameLog The game log.
    ///
    virtual void gameEnd(const GameLog &gameLog) = 0;

    /// Shutdown this agent.
    ///
    virtual void shutdown() = 0;
};


using PlayerAgents = std::array<AgentPtr, Player::count>;

