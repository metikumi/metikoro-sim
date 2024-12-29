// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Utilities.hpp"

#include <cstdint>


class BoardArea {
public:
    enum Value : uint8_t {
        Player = 0, ///< Playable area
        Frame, ///< Frame with no function.
        House, ///< Home area.
        Garden, ///< Garden area.
        Source, ///< The source.
    };

public:
    constexpr BoardArea() = default;
    constexpr BoardArea(Value value) : _value{value} {};

public: // operators
    [[nodiscard]] auto operator==(const BoardArea &other) const noexcept -> bool = default;
    [[nodiscard]] auto operator==(const Value value) const noexcept -> bool { return _value == value; }
    [[nodiscard]] operator uint8_t() const noexcept { return static_cast<uint8_t>(_value); }

public:
    [[nodiscard]] auto value() const noexcept -> Value { return _value; }

public:
    [[nodiscard]] constexpr auto toChar() const noexcept -> char {
        switch (_value) {
        case Frame:
            return 'F';
        case House:
            return 'H';
        case Garden:
            return 'G';
        case Source:
            return 'S';
        case Player:
        default:
            return ' ';
        }
    }

private:
    Value _value{Player};
};


template<>
struct std::hash<BoardArea> {
    auto operator()(const BoardArea &boardArea) const noexcept -> std::size_t {
        return utility::hashFromValues(boardArea);
    }
};

