// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Anchors.hpp"
#include "Field.hpp"
#include "OrbTravelSegment.hpp"
#include "OrbTravelPoint.hpp"


/// A node for orb travel calculation.
///
class OrbTravelNode {
public:
    /// Create a dead-end.
    OrbTravelNode() = default;

public: // Accessors
    [[nodiscard]] auto travel() const noexcept -> const OrbTravelSegment& { return _travel; }
    [[nodiscard]] auto position() const noexcept -> Position { return _travel.position(); }
    [[nodiscard]] auto begin() const noexcept -> Anchor { return _travel.begin(); }
    [[nodiscard]] auto end() const noexcept -> Anchor { return _travel.end(); }
    [[nodiscard]] auto beginPoint() const noexcept ->OrbTravelPoint { return _travel.beginPoint(); }
    [[nodiscard]] auto endPoint() const noexcept -> OrbTravelPoint { return _travel.endPoint(); }
    [[nodiscard]] auto nextPoint() const noexcept -> OrbTravelPoint { return _travel.nextPoint(); }

public: // tests
    [[nodiscard]] constexpr auto reachedStop() const noexcept -> bool { return _travel.reachedStop(); }
    [[nodiscard]] constexpr auto isDeadEnd() const noexcept -> bool { return _travel.isDeadEnd(); }
    [[nodiscard]] constexpr auto hasOptions() const noexcept -> bool { return not _options.empty(); }
    [[nodiscard]] constexpr auto canTravelForward() const noexcept -> bool {
        return _options.contains(_travel.end());
    }

public: // modifiers
    /// Select the next option if possible.
    ///
    /// @return `true` if there is a next option, `false` if no option is left.
    ///
    [[nodiscard]] auto selectNextOption() noexcept -> bool {
        _options.remove(_travel.end());
        if (_options.empty()) {
            _travel.setEnd(Anchor::Stop);
            return false;
        }
        _travel.setEnd(_options.first());
        return true;
    }

    /// Remove the current option from this node.
    ///
    void removeCurrentOption() noexcept {
        _options.remove(_travel.end());
    }

    [[nodiscard]] auto toString() const noexcept -> std::string {
        if (isDeadEnd()) {
            return "OrbTravelNode(dead end)";
        }
        return std::format("OrbTravelNode({}, options=[{}])", _travel.toString(), _options.toString());
    }

public:
    [[nodiscard]] static auto from(
        const OrbTravelPoint &point,
        const Field field) noexcept -> OrbTravelNode {

        const auto connections = field.connectionsFrom(point.anchor());
        if (connections.empty()) {
            return {};
        }
        return OrbTravelNode{
            OrbTravelSegment{point.position(), point.anchor(), connections.first()},
            connections};
    }

private:
    /// Create a new travel node.
    constexpr OrbTravelNode(OrbTravelSegment travel, Anchors alternativeEnds)
        : _travel{travel}, _options{alternativeEnds} {
    }

private:
    OrbTravelSegment _travel{};
    Anchors _options{};
};


using OrbTravelNodeStack = std::vector<OrbTravelNode>;
