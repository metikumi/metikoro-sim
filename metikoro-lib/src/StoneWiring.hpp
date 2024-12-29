// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Anchors.hpp"
#include "Orientations.hpp"
#include "StoneElement.hpp"


class StoneWiringTest;


class StoneWiring {
    friend class StoneWiringTest; // make private methods accessible for testing.

public:
    using Connections = std::array<Anchors, Anchor::count>;

public:
    StoneWiring() = default;
    explicit StoneWiring(const StoneElement element) noexcept {
        const auto [source, target] = element.connection();
        connections[source.value()] = target;
        connections[target.value()] |= source;
        updateUniqueOrientations();
    }
    auto operator|=(const StoneElement element) noexcept -> StoneWiring& {
        const auto [source, target] = element.connection();
        connections[source.value()] |= target;
        connections[target.value()] |= source;
        updateUniqueOrientations();
        return *this;
    }
    auto operator|(const StoneElement element) const noexcept -> StoneWiring {
        StoneWiring result = *this;
        result |= element;
        return result;
    }

    [[nodiscard]] auto hasStop() const noexcept -> bool {
        return not connections[Anchor::Stop].empty();
    }

    [[nodiscard]] auto isEqual(const Orientation orientationA, const Orientation orientationB) const noexcept -> bool {
        return rotatedConnections(orientationA.toRotation()) == rotatedConnections(orientationB.toRotation());
    }

private:
    [[nodiscard]] auto rotatedConnections(const Rotation rotation) const noexcept -> Connections {
        if (rotation == Rotation::None) {
            return connections;
        }
        Connections result{};
        for (const auto cp : Anchor::all()) {
            result[cp.rotated(rotation).value()] = connections[cp.value()].rotated(rotation);
        }
        return result;
    }

    void updateUniqueOrientations() {
        uniqueOrientations = Orientation{Orientation::North};
        if (not isEqual(Orientation::North, Orientation::East)) {
            uniqueOrientations |= Orientation{Orientation::East};
        }
        if (not isEqual(Orientation::North, Orientation::South)) {
            uniqueOrientations |= Orientation{Orientation::South};
        }
        if (not isEqual(Orientation::North, Orientation::West) && not isEqual(Orientation::East, Orientation::West)) {
            uniqueOrientations |= Orientation{Orientation::West};
        }
    }

public:
    Connections connections{};
    Orientations uniqueOrientations{Orientation::North};
};


inline auto operator|(const StoneElement &lhs, const StoneElement &rhs) noexcept -> StoneWiring {
    return StoneWiring{rhs} | lhs;
}

