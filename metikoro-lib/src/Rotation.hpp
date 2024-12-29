// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Utilities.hpp"

#include <cassert>
#include <cstdint>
#include <vector>


/// Rotation angle.
///
class Rotation {
public:
    static constexpr int8_t maximum = 4;
    enum Value : int8_t {
        None = 0,
        Clockwise90 = 1,
        Clockwise180 = 2,
        Clockwise270 = 3,
        CounterClockwise90 = -1,
        CounterClockwise180 = -2,
        CounterClockwise270 = -3,
    };

private:
    static constexpr auto wrapValue(int value) noexcept -> int8_t {
        while (value <= -maximum) {
            value += maximum;
        }
        while (value >= maximum) {
            value -= maximum;
        }
        return static_cast<int8_t>(value);
    }
    static constexpr auto wrapValueToValue(const int value) noexcept -> Value {
        return static_cast<Value>(wrapValue(value));
    };
    static constexpr auto wrapValueToRotation(const int value) noexcept -> Rotation {
        return Rotation{wrapValueToValue(value)};
    };

public:
    Rotation() = default;
    constexpr Rotation(Value value) noexcept : _value{value} {};

    ALL_COMPARE_OPERATORS(Rotation, _value)
    ALL_COMPARE_WITH_NATIVE_OPERATORS(Value, _value);

    constexpr operator int8_t() const noexcept { return _value; }

    constexpr auto operator+(const Rotation rotation) const -> Rotation {
        return wrapValueToRotation(_value + rotation._value);
    }
    constexpr auto operator-(const Rotation rotation) const -> Rotation {
        return wrapValueToRotation(_value - rotation._value);;
    }
    constexpr auto operator+=(Rotation rotation) -> Rotation& {
        _value = wrapValueToValue(_value + rotation._value);
        return *this;
    }
    constexpr auto operator-=(Rotation rotation) -> Rotation& {
        _value = wrapValueToValue(_value - rotation._value);
        return *this;
    }
    constexpr auto operator++() -> Rotation& {
        _value = wrapValueToValue(_value + 1);
        return *this;
    }
    constexpr auto operator--() -> Rotation& {
        _value = wrapValueToValue(_value - 1);
        return *this;
    }
    constexpr auto operator++(int) -> Rotation {
        auto copy = *this;
        _value = wrapValueToValue(_value + 1);
        return copy;
    }
    constexpr auto operator--(int) -> Rotation {
        auto copy = *this;
        _value = wrapValueToValue(_value - 1);
        return copy;
    }

public:
    [[nodiscard]] constexpr auto value() const noexcept -> Value { return _value; }
    [[nodiscard]] constexpr auto isNone() const noexcept -> bool { return _value == None; }
    [[nodiscard]] constexpr auto isClockwise() const noexcept -> bool { return _value >= 0; }
    [[nodiscard]] constexpr auto isCounterClockwise() const noexcept -> bool { return _value < 0; }
    [[nodiscard]] constexpr auto reversed() const noexcept -> Rotation {
        return {static_cast<Value>(-static_cast<int8_t>(_value))};
    }
    [[nodiscard]] constexpr auto wrapToClockwise() const noexcept -> Rotation {
        if (isClockwise()) {
            return *this;
        }
        return wrapValueToRotation(_value + maximum);
    }
    [[nodiscard]] auto toString() const noexcept -> std::string {
        switch (_value) {
        case None: return "0º CW";
        case Clockwise90: return "90º CW";
        case Clockwise180: return "180º CW";
        case Clockwise270: return "270º CW";
        case CounterClockwise90: return "90º CCW";
        case CounterClockwise180: return "180º CCW";
        case CounterClockwise270: return "270º CCW";
        default: return {};
        }
    }

public:
    [[nodiscard]] constexpr static auto allClockwise() -> const std::array<Rotation, maximum>& {
        static constexpr std::array<Rotation, maximum> all{None, Clockwise90, Clockwise180, Clockwise270};
        return all;
    }

private:
    Value _value{None};
};


using RotationPair = std::pair<Rotation, Rotation>;


template<>
struct std::hash<Rotation> {
    auto operator()(const Rotation &rotation) const noexcept -> std::size_t {
        return std::hash<Rotation::Value>{}(rotation.value());
    }
};
