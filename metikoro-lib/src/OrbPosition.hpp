// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Position.hpp"


class OrbPosition {
public:
    OrbPosition() = default;

public:
    [[nodiscard]] auto operator==(const OrbPosition &other) const -> bool = default;

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return Position::dataSize() + 1U + Position::dataSize();
    }

    void addToData(std::string &data) const noexcept {
        position.addToData(data);
        if (koLock == 0) {
            data.append("___");
        } else {
            data += utility::valueToHexDigit(koLock);
            koPosition.addToData(data);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> OrbPosition {
        if (data.size() != dataSize()) {
            throw Error("OrbPosition: Invalid data size.");
        }
        OrbPosition result;
        result.position = Position::fromData(data.substr(0, Position::dataSize()));
        if (data[Position::dataSize()] == '_') {
            result.koLock = 0;
            result.koPosition = Position::invalid();
        } else {
            result.koLock = utility::hexDigitToValue(data[Position::dataSize()]);
            result.koPosition = Position::fromData(data.substr(Position::dataSize() + 1U, Position::dataSize()));
        }
        return result;
    }

public:
    Position position{Position::invalid()};
    uint8_t koLock{0};
    Position koPosition{Position::invalid()};
};


static_assert(Serializable<OrbPosition>);


template<>
struct std::hash<OrbPosition> {
    auto operator()(const OrbPosition &orbPosition) const noexcept -> std::size_t {
        return utility::hashFromValues(orbPosition.position, orbPosition.koLock, orbPosition.koPosition);
    }
};

