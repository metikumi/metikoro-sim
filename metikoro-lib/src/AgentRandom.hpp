// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <random>

#include "Agent.hpp"


class AgentRandom;
using AgentRandomPtr = std::shared_ptr<AgentRandom>;


/// A random player.
///
class AgentRandom final : public Agent {
    constexpr static auto allowNoActions = false;
    constexpr static auto allowNoDraw = false;

public:
    AgentRandom() = default;
    ~AgentRandom() override = default;

    /// Copy a random agent.
    ///
    /// Only copy the configuration, but create a new individual RNG.
    ///
    AgentRandom(const AgentRandom &copy) : _seed{copy._seed} {
    }

private:
    template<typename T>
    auto selectRandom(const T &elements) {
        if (elements.size() == 0) {
            throw Error("AgentRandom: Empty elements container to choose from.");
        }
        if (elements.size() == 1) {
            return elements.at(0);
        }
        std::uniform_int_distribution<std::size_t> actionsDist(0, elements.size() - 1);
        return elements.at(actionsDist(_rng));
    }

    void initializeRngFromSeed() noexcept {
        if (_seed == 0) {
            _rng.seed(std::random_device{}());
        } else {
            _rng.seed(_seed);
        }
    }

public:
    [[nodiscard]] static auto getHelp() noexcept -> std::string {
        return "  --seed=<rng seed>    A positive 64-bit number as seed for the prng. 0 = random seed.";
    }

    void initialize(std::span<std::string_view> args) override {
        for (const auto arg : args) {
            if (arg.starts_with("--seed=")) {
                _seed = std::stoul(std::string{arg.substr(arg.find_first_of('=') + 1)});
            } else {
                throw Error{"Unknown random agent option: " + std::string{arg}};
            }
        }
        initializeRngFromSeed();
    }

    auto configurationString() const noexcept -> std::string override {
        std::stringstream result;
        result << "seed = ";
        if (_seed == 0) {
            result << "random";
        } else {
            result << _seed;
        }
        return result.str();
    }

    auto copyForThread() noexcept -> AgentPtr override {
        auto copy = std::make_shared<AgentRandom>(*this);
        copy->initializeRngFromSeed();
        return copy;
    }

    void gameStart() override {
        // not used.
    }

    [[nodiscard]] auto nextMove(const GameState &state, const GameLog& /*gameLog*/) -> GameMove override {
        const auto allActions = state.allActions();
        auto tempState = state;
        ActionSequence actionSequence;
        if (allActions.empty()) {
            if constexpr (not allowNoActions) {
                throw Error("AgentRandom: There was no possible action to select from.");
            }
            actionSequence = {};
        } else {
            actionSequence = selectRandom(allActions.actions());
            actionSequence.applyTo(tempState);
        }
        const auto allRegularDraws = tempState.allRegularDraws();
        Stone drawStone;
        if (allRegularDraws.empty()) {
            if constexpr (not allowNoDraw) {
                throw Error("AgentRandom: There was no possible draw to select from.");
            }
            drawStone = {};
        } else {
            drawStone = selectRandom(tempState.allRegularDraws());
        }
        const auto orbMove = selectRandom(tempState.allOrbMoves());
        return GameMove{actionSequence, drawStone, orbMove};
    }

    void gameEnd(const GameLog& /*gameLog*/) override {
        // not used.
    }

    void shutdown() override {
        // not used.
    }

private:
    // configuration
    std::size_t _seed{0};

    // working
    std::mt19937 _rng{};
};

