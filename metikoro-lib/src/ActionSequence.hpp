// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Action.hpp"
#include "Error.hpp"


class GameState;


class ActionSequence {
    friend struct std::hash<ActionSequence>;

public:
    using Sequence = std::array<Action, Action::maximumPerMove>;

public:
    ActionSequence() = default;
    constexpr ActionSequence(const Action &action) noexcept // NOLINT(*-explicit-constructor)
        : _sequence({action, {}}) {
    }
    constexpr ActionSequence(const Sequence &sequence) noexcept // NOLINT(*-explicit-constructor)
        : _sequence{sequence} {
    }

public: // operators
    [[nodiscard]] auto operator==(const ActionSequence &other) const -> bool = default;

public:
    /// Test if this sequence contains no actions.
    ///
    [[nodiscard]] auto hasNoActions() const noexcept {
        return _sequence[0].isNone();
    }

    /// Access the underlying action sequence.
    ///
    [[nodiscard]] auto sequence() const noexcept -> const Sequence& { return _sequence; }

    /// Execute all actions from this action sequence for the given state.
    ///
    void applyTo(GameState &state) const {
        for (const Action &action : sequence()) {
            if (not action.isNone()) {
                action.applyTo(state);
            }
        }
    }

    /// Create a string for this sequence for debugging.
    ///
    [[nodiscard]] auto toString() const noexcept -> std::string {
        if (hasNoActions()) {
            return "ActionSequence(no actions)";
        }
        std::stringstream result;
        result << "ActionSequence(";
        bool first = true;
        for (const auto action : _sequence) {
            if (action.type() == Action::None) {
                break;
            }
            if (not first) {
                result << ", ";
            }
            result << action.toString();
            first = false;
        }
        result << ")";
        return result.str();
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return Action::dataSize() * Action::maximumPerMove;
    }

    void addToData(std::string &data) const noexcept {
        for (const auto action : _sequence) {
            action.addToData(data);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> ActionSequence {
        if (data.size() != dataSize()) {
            throw Error("ActionSequence: Invalid data size.");
        }
        ActionSequence result;
        for (std::size_t i = 0; i < result._sequence.size(); ++i) {
            result._sequence.at(i) = Action::fromData(data.substr(i * Action::dataSize(), Action::dataSize()));
        }
        return result;
    }

private:
    Sequence _sequence{};
};


static_assert(Serializable<ActionSequence>);


template<>
struct std::hash<ActionSequence> {
    auto operator()(const ActionSequence &gameActionSequence) const noexcept -> std::size_t {
        return utility::hashFromArray(gameActionSequence._sequence);
    }
};

