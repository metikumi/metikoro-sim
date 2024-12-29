// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Rotation.hpp"
#include "Serializable.hpp"

#include <cstdint>
#include <format>


using Length = uint8_t;


class Position {
    friend struct std::hash<Position>;

public:
    static constexpr Length maxLength = 0x0fU;
    struct Data {
        Length x : 4 = 0;
        Length y : 4 = 0;
    };

public:
    Position() = default;
    constexpr Position(const Length x, const Length y) noexcept : _data{.x = x, .y = y} {};

public: // operators
    [[nodiscard]] constexpr auto operator==(const Position &other) const -> bool {
        return _data.x == other._data.x && _data.y == other._data.y;
    }
    [[nodiscard]] constexpr auto operator!=(const Position &other) const -> bool { return not (*this == other); }
    [[nodiscard]] constexpr auto operator<(const Position &other) const -> bool {
        return _data.y < other._data.y || (_data.y == other._data.y && _data.x < other._data.x);
    }
    [[nodiscard]] constexpr auto operator<=(const Position &other) const -> bool {
        return operator<(other) || operator==(other);
    }
    [[nodiscard]] constexpr auto operator>(const Position &other) const -> bool {
        return _data.y > other._data.y || (_data.y == other._data.y && _data.x > other._data.x);
    }
    [[nodiscard]] constexpr auto operator>=(const Position &other) const -> bool {
        return operator>(other) || operator==(other);
    }
    [[nodiscard]] auto operator-(const Position &other) const -> Position {
        return {static_cast<Length>(_data.x - other._data.x), static_cast<Length>(_data.y - other._data.y)};
    }
    [[nodiscard]] auto operator+(const Position &other) const -> Position {
        return {static_cast<Length>(_data.x + other._data.x), static_cast<Length>(_data.y + other._data.y)};
    }

public: // attributes
    [[nodiscard]] auto x() const -> Length { return _data.x; }
    [[nodiscard]] auto y() const -> Length { return _data.y; }

public: // methods
    [[nodiscard]] auto isInvalid() const -> bool { return _data.x == maxLength || _data.y == maxLength; }
    [[nodiscard]] auto rotated(const Rotation rotation, const Length size) const -> Position {
        if (isInvalid()) { return invalid(); }
        switch (rotation) {
        case Rotation::None: return *this;
        case Rotation::Clockwise90: return {_data.y, static_cast<Length>(size - 1U - _data.x)};
        case Rotation::Clockwise180: return {static_cast<Length>(size - 1U - _data.y), static_cast<Length>(size - 1U - _data.x)};
        case Rotation::Clockwise270: return {static_cast<Length>(size - 1U - _data.x), _data.y};
        default: return *this;
        }
    }

    [[nodiscard]] auto toString(bool withBrackets = true) const noexcept -> std::string {
        if (withBrackets) {
            return std::format("[{},{}]", _data.x, _data.y);
        }
        return std::format("{},{}", _data.x, _data.y);
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return 2U;
    }

    void addToData(std::string &data) const noexcept {
        if (isInvalid()) {
            data.append(dataSize(), '_');
        } else {
            data += utility::valueToHexDigit(_data.x);
            data += utility::valueToHexDigit(_data.y);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> Position {
        if (data.size() != dataSize() or not utility::isHexDigit(data.at(0)) or not utility::isHexDigit(data.at(1))) {
            return invalid();
        }
        return {utility::hexDigitToValue(data.at(0)), utility::hexDigitToValue(data.at(1))};
    }

public:
    static constexpr auto invalid() noexcept -> Position { return Position{maxLength, maxLength}; };

private:
    Data _data;
};


static_assert(Serializable<Position>);


using PositionList = std::vector<Position>;
using PositionPair = std::pair<Position, Position>;
using PositionPairList = std::vector<PositionPair>;


template<>
struct std::hash<Position> {
    auto operator()(const Position &position) const noexcept -> std::size_t {
        std::size_t seed{};
        utility::combineHash(seed, position._data.x);
        utility::combineHash(seed, position._data.y);
        return seed;
    }
};

