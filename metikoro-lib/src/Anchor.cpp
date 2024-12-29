// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#include "Anchor.hpp"


#include "OrbTravelPoint.hpp"


auto Anchor::nextPoint(const Position pos) const noexcept -> OrbTravelPoint {
    switch (_value) {
    case North: return {pos - Position{0, 1}, South};
    case East: return {pos + Position{1, 0}, West};
    case South: return {pos + Position{0, 1}, North};
    case West: return {pos - Position{1, 0}, East};
    case Stop:
    default:
        return {Position::invalid(), Stop};
    }
}
