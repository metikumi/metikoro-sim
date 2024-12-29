// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once



#include "Anchor.hpp"
#include "Position.hpp"


/// A single point along the travel path of an orb.
///
class OrbTravelPoint {
public:
    OrbTravelPoint() = default;
    constexpr OrbTravelPoint(Position position, Anchor anchor) noexcept : _position{position}, _anchor{anchor} {
    }

public: // operators
    [[nodiscard]] auto operator==(const OrbTravelPoint &other) const noexcept -> bool = default;

public:
    [[nodiscard]] auto position() const noexcept -> Position { return _position; }
    [[nodiscard]] auto anchor() const noexcept -> Anchor { return _anchor; }

    [[nodiscard]] auto toString() const noexcept -> std::string {
        return std::format("OrbTravelPoint({}, {})", _position.toString(), _anchor.toString());
    }

private:
    Position _position;
    Anchor _anchor;
};

