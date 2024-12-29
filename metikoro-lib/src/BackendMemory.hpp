// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Backend.hpp"
#include "GameLog.hpp"
#include "GameState.hpp"
#include "RatingGame.hpp"

#include <thread>
#include <unordered_map>



class BackendMemory final : public Backend {
public:
    BackendMemory() = default;

public:
    [[nodiscard]] static auto getHelp() noexcept -> std::string {
        return "  no options";
    }

    void initialize(std::span<std::string_view> args) override {
        for (const auto &arg : args) {
            throw Error{"Unknown memory backend option: " + std::string{arg}};
        }
    }

    void load() override {
        // unused
    }

    void addGame(const GameLog &gameLog) override {
        if (gameLog.empty()) {
            return;
        }
        std::unique_lock const lock(_libraryMutex);
        auto adjustments = gameLog.createRatingAdjustments();
        if (gameLog.size() != adjustments.size()) {
            throw Error("Adjustments do not match game log size.");
        }
        for (const auto &[turn, adjustment] : std::views::zip(gameLog, adjustments)) {
            _gameStates[turn.state].applyAdjustment(adjustment);
        }
    }

    void shutdown() override {
        // unused
    }

private:
    std::mutex _libraryMutex; ///< The mutex to allow multithreaded access.
    std::unordered_map<GameState, RatingGame> _gameStates; ///< All known game states.
};

