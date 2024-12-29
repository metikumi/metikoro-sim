// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ActionSequence.hpp"


class GameState;


class ActionSequences {
public:
    using Actions = std::vector<ActionSequence>;

public:
    ActionSequences() = default;

public:
    [[nodiscard]] auto actions() const noexcept -> const Actions& {
        return _actions;
    }
    [[nodiscard]] auto empty() const noexcept -> bool {
        return _actions.empty();
    }
    void reserve(const std::size_t size) noexcept {
        _actions.reserve(size);
    }
    void add(const ActionSequence &actionSequence) noexcept {
        _actions.emplace_back(actionSequence);
    }

    /// Generate all valid action sequences for the given state.
    ///
    [[nodiscard]] static auto allForState(const GameState &state) noexcept -> ActionSequences;

private:
    std::vector<ActionSequence> _actions;
};

