// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Anchors.hpp"
#include "OrbTravelPoint.hpp"
#include "Position.hpp"
#include "Stone.hpp"


/// Travel along a segment on a stone.
///
class OrbTravelSegment {
public:
    OrbTravelSegment() = default;
    constexpr OrbTravelSegment(const Position position, const Anchor begin, const Anchor end)
        : _position{position}, _begin{begin}, _end{end} {
    }

public: // operators
    [[nodiscard]] auto operator==(const OrbTravelSegment &other) const noexcept -> bool = default;

public: // accessors
    [[nodiscard]] auto position() const noexcept -> Position { return _position; }
    [[nodiscard]] auto begin() const noexcept -> Anchor { return _begin; }
    [[nodiscard]] auto end() const noexcept -> Anchor { return _end; }
    [[nodiscard]] auto beginPoint() const noexcept ->OrbTravelPoint {
        return OrbTravelPoint{_position, _begin};
    }
    [[nodiscard]] auto endPoint() const noexcept -> OrbTravelPoint {
        return OrbTravelPoint{_position, _end};
    }
    [[nodiscard]] auto nextPoint() const noexcept -> OrbTravelPoint {
        return end().nextPoint(position());
    }
    [[nodiscard]] auto toString() const noexcept -> std::string {
        if (isDeadEnd()) {
            return "OrbTravel(dead end)";
        }
        return std::format("OrbTravel(pos={}, {}=>{})", _position.toString(), _begin.toString(), _end.toString());
    }

public: // tests
    [[nodiscard]] constexpr auto reachedStop() const noexcept -> bool { return end() == Anchor::Stop; }
    [[nodiscard]] constexpr auto isDeadEnd() const noexcept -> bool { return _position == Position::invalid(); }

public: // modifiers
    void setEnd(const Anchor end) noexcept { _end = end; }

private:
    Position _position;
    Anchor _begin;
    Anchor _end;
};

