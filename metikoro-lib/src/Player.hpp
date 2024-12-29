// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Rotation.hpp"

#include <cstdint>


class Player {
public:
    static constexpr uint8_t count = 4;

public:
    Player() = default;
    constexpr explicit Player(const uint8_t value) noexcept : _value{value} {};

public: // operators
    ALL_COMPARE_OPERATORS(Player, _value);
    ALL_COMPARE_WITH_NATIVE_OPERATORS(uint8_t, _value);

public: // attributes
    [[nodiscard]] auto value() const noexcept -> uint8_t { return _value; }
    [[nodiscard]] auto flag() const noexcept -> uint8_t { return 1U << _value; }
    [[nodiscard]] constexpr operator uint8_t() const noexcept { return _value; }

public: // modifier
    /// Offset a player with another one.
    ///
    /// E.g. on a state, where the active player is normalized to the top left corner (player 0),
    /// the player 1 has won. Yet, in a game, the active player for this state was player 2. In this case,
    /// the actual winning player was player 3.
    ///
    /// `activePlayer.offsetWith(state.winningPlayer) => real winner`
    ///
    [[nodiscard]] auto offsetWith(Player player) const noexcept -> Player {
        return Player{static_cast<uint8_t>((_value + player.value()) % count)};
    }

    /// Advance this to the next player in the game.
    ///
    void next() noexcept {
        _value = (_value + 1) % count;
    }

    /// Advance this to the previous player in the game.
    ///
    void previous() noexcept {
        _value = (_value + count - 1) % count;
    }

public: // conversion
    [[nodiscard]] constexpr auto toChar() const noexcept -> char {
        return '0' + _value;
    }

    static auto all() noexcept -> const std::array<Player, count>& {
        static constexpr std::array<Player, count> all{Player{0}, Player{1}, Player{2}, Player{3}};
        return all;
    }

private:
    uint8_t _value{0};
};

