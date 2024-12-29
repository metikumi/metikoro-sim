// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Serializable.hpp"
#include "Rotation.hpp"
#include "Utilities.hpp"

#include <cstdint>

#include "Error.hpp"


/// The orientation of a stone on the board.
///
class Orientation {
public:
    static constexpr uint8_t count = 4;
    enum Value : uint8_t {
        North = 0U,
        East = 1U,
        South = 2U,
        West = 3U,
    };

public:
    Orientation() = default;
    constexpr Orientation(Value value) noexcept : _value{value} {};

    ALL_COMPARE_OPERATORS(Orientation, _value)
    ALL_COMPARE_WITH_NATIVE_OPERATORS(Value, _value)

    constexpr operator uint8_t() const noexcept { return _value; }

    constexpr auto operator+(const Rotation rotation) const noexcept -> Orientation {
        return Orientation{static_cast<Value>((_value + count + rotation.value()) % count)};
    }
    constexpr auto operator+(const Rotation::Value rotation) const noexcept -> Orientation {
        return Orientation{static_cast<Value>((_value + count + rotation) % count)};
    }
    constexpr auto operator-(const Rotation rotation) const noexcept -> Orientation {
        return Orientation{static_cast<Value>((_value + count - rotation.value()) % count)};
    }
    constexpr auto operator-(const Rotation::Value rotation) const noexcept -> Orientation {
        return Orientation{static_cast<Value>((_value + count - rotation) % count)};
    }
    auto operator+=(const Rotation rotation) noexcept -> Orientation& {
        _value = operator+(rotation).value();
        return *this;
    }
    auto operator+=(const Rotation::Value rotation) noexcept -> Orientation& {
        _value = operator+(rotation).value();
        return *this;
    }
    auto operator-=(const Rotation rotation) noexcept -> Orientation& {
        _value = operator-(rotation).value();
        return *this;
    }
    auto operator-=(const Rotation::Value rotation) noexcept -> Orientation& {
        _value = operator-(rotation).value();
        return *this;
    }

public:
    [[nodiscard]] constexpr auto value() const noexcept -> Value { return _value; }
    [[nodiscard]] constexpr auto flag() const noexcept -> uint8_t { return 1U << _value; }
    [[nodiscard]] constexpr auto toRotation() const noexcept -> Rotation {
        return Rotation{static_cast<Rotation::Value>(_value)};
    }
    [[nodiscard]] auto toString() const noexcept -> std::string {
        switch (_value) {
        case North: return "N";
        case East: return "E";
        case South: return "S";
        case West: return "W";
        default: return {};
        }
    }
    [[nodiscard]] auto toArrow() const noexcept -> std::string {
        switch (_value) {
        case North: return "↑";
        case East: return "→";
        case South: return "↓";
        case West: return "←";
        default: return " ";
        }
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return 1U;
    }

    void addToData(std::string &data) const noexcept {
        data.append(toString());
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> Orientation {
        if (data.size() != dataSize()) {
            throw Error("Orientation: Invalid data size.");
        }
        switch (data.at(0)) {
        case 'N': return North;
        case 'E': return East;
        case 'S': return South;
        case 'W': return West;
        default: return North;
        }
    }

public:
    [[nodiscard]] constexpr static auto all() noexcept -> const std::array<Orientation, count>& {
        static constexpr std::array<Orientation, count> all{North, East, South, West};
        return all;
    }

private:
    Value _value{North};
};


static_assert(Serializable<Orientation>);


using OrientationPair = std::pair<Orientation, Orientation>;


template<>
struct std::hash<Orientation> {
    auto operator()(const Orientation &orientation) const noexcept -> std::size_t {
        return std::hash<Orientation::Value>{}(orientation.value());
    }
};

