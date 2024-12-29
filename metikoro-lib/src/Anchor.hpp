// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Orientation.hpp"
#include "Position.hpp"
#include "Rotation.hpp"

#include <cstdint>
#include <vector>


class OrbTravelPoint;


class Anchor {
public:
    static constexpr uint8_t count = 5;
    static constexpr uint8_t sideCount = 4;
    static constexpr uint8_t sideMask = 0x0fU;
    static constexpr uint8_t centerMask = 0x10U;

public:
    enum Value : uint8_t {
        North = 0U,
        East = 1U,
        South = 2U,
        West = 3U,
        Stop = 4U,
    };

public:
    constexpr Anchor() = default;
    constexpr Anchor(Value value) : _value(value) {}

    ALL_COMPARE_OPERATORS(Anchor, _value);
    ALL_COMPARE_WITH_NATIVE_OPERATORS(Value, _value);

    [[nodiscard]] constexpr auto value() const -> Value { return _value; }
    [[nodiscard]] constexpr auto flag() const -> uint8_t { return static_cast<uint8_t>(1U) << static_cast<uint8_t>(_value); }

    /// Rotate the connection into the given direction.
    ///
    [[nodiscard]] constexpr auto rotated(Rotation rotation) const -> Anchor {
        return std::array<Value, Rotation::maximum * count>{
            North, East, South, West, Stop,
            East, South, West, North, Stop,
            South, West, North, East, Stop,
            West, North, East, South, Stop
        }.at(rotation.wrapToClockwise().value() * count + _value);
    }

    /// Rotated for a given orientation.
    ///
    /// E.g. for orientation N : N => N, E => E ... O = O
    /// for orientation E : N => E, E => S, ... O = O
    ///
    [[nodiscard]] constexpr auto rotated(const Orientation orientation) const noexcept -> Anchor {
        return rotated(orientation.toRotation());
    }

    /// Return the anchor, normalized for the given stone orientation.
    ///
    /// E.g. for orientation N : N => N, E => E ... O = O
    /// for orientation E : N => W, E => N, ... O = O
    ///
    [[nodiscard]] constexpr auto normalized(const Orientation orientation) const noexcept -> Anchor {
        return rotated(orientation.toRotation().reversed());
    }

    /// Get the next logical travel point from the given position.
    ///
    [[nodiscard]] auto nextPoint(Position pos) const noexcept -> OrbTravelPoint;

    /// Return a string from this anchor.
    ///
    [[nodiscard]] auto toString() const noexcept -> std::string {
        switch (_value) {
        case North: return "N";
        case East: return "E";
        case South: return "S";
        case West: return "W";
        case Stop: return "O";
        default: return "-";
        }
    }

public:
    static constexpr auto all() noexcept -> const std::array<Anchor, count>& {
        static constexpr std::array<Anchor, count> all{North, East, South, West, Stop};
        return all;
    }


private:
    Value _value{North};
};



