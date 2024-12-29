// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Anchors.hpp"
#include "Orientation.hpp"
#include "StoneWiring.hpp"
#include "Utilities.hpp"

#include <sstream>
#include <vector>


// _: ┌─────┐
//    │     │
//    │     │
//    │     │
//    └─────┘
// A: ┌──┬──┐ B: ┌──┬──┐ C: ┌──┬──┐ D: ┌──┬──┐ E: ┌──┬──┐ F: ┌──┬──┐ G: ┌──┬──┐
//    │  │  │    │  │  │    │  ╰╮ │    │  ├╮ │    │ ╭┼╮ │    │ ╭┴╮ │    │ ╭╯  │
//    ├──┼──┤    ├──○──┤    ├─╮ ╰─┤    ├─╮│╰─┤    ├─╯│╰─┤    ├─┤ ├─┤    ├─╯ ▷─┤
//    │  │  │    │  │  │    │ ╰╮  │    │ ╰┤  │    │  │  │    │ ╰┬╯ │    │  ▽  │
//    └──┴──┘    └──┴──┘    └──┴──┘    └──┴──┘    └──┴──┘    └──┴──┘    └──┴──┘
// H: ┌──┬──┐ I: ┌──┬──┐ J: ┌──┬──┐
//    │  │  │    │  │  │    │  ╰╮ │
//    │  ○──┤    │  ○──┤    │   ╰─┤
//    │  │  │    │     │    │     │
//    └──┴──┘    └─────┘    └─────┘

class Stone {
public:
    enum Format : uint8_t {
        Short = 0,
        Long,
    };
    enum Type : uint8_t {
        Empty = 0, // _
        Crossing, // A
        CrossingWithStop, // B
        TwoCurves, // C
        SwitchA, // D
        SwitchB, // E
        SwitchC, // F
        CurveWithBounces, // G
        SwitchWithStop, // H
        OneCurveWithStop, // I
        OneCurve, // J

        EnumCount, // must be last element
    };
    constexpr static uint8_t count = EnumCount;
    using Wiring = std::array<const StoneWiring, count>;

public:
    Stone() = default;
    constexpr Stone(Type type) noexcept : _type(type) {}
    constexpr Stone(uint8_t typeInt) noexcept : _type(static_cast<Type>(typeInt)) {}

    ALL_COMPARE_OPERATORS(Stone, _type);
    ALL_COMPARE_WITH_NATIVE_OPERATORS(Type, _type);

    explicit operator uint8_t() const noexcept { return static_cast<uint8_t>(_type); }

public:
    [[nodiscard]] constexpr auto type() const noexcept -> Type {
        return _type;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return _type == Empty;
    }

    [[nodiscard]] constexpr auto hasStop() const noexcept -> bool {
        return thisWiring().hasStop();
    }

    [[nodiscard]] constexpr auto connectionsFrom(const Anchor side) const noexcept -> Anchors {
        return thisWiring().connections.at(side.value());
    }

    [[nodiscard]] constexpr auto uniqueOrientations() const noexcept -> Orientations {
        return thisWiring().uniqueOrientations;
    }

    [[nodiscard]] auto isEqual(const Orientation orientationA, const Orientation orientationB) const noexcept -> bool {
        if (orientationA == orientationB) {
            return true;
        }
        return thisWiring().isEqual(orientationA, orientationB);
    }

    [[nodiscard]] constexpr auto canRotate() const -> bool {
        return not uniqueOrientations().empty() && uniqueOrientations() != Orientations{Orientation::North};
    }

    [[nodiscard]] constexpr auto allOrientationsAreUnique() const -> bool {
        return uniqueOrientations() == Orientations{Orientation::North, Orientation::East, Orientation::South, Orientation::West};
    }

    /// Normalize the orientation for this type of stone.
    ///
    [[nodiscard]] constexpr auto normalizedOrientation(Orientation orientation) const -> Orientation {
        if (empty() or not canRotate()) {
            orientation = Orientation::North;
        } else if (not allOrientationsAreUnique() and (orientation == Orientation::South or orientation == Orientation::West)) {
            orientation += Rotation::Clockwise180; // flip back
        }
        return orientation;
    }

public: // collections
    [[nodiscard]] static constexpr auto all() noexcept -> std::array<Stone, count> {
        return {Empty, Crossing, CrossingWithStop, TwoCurves, SwitchA, SwitchB, SwitchC,
            CurveWithBounces, SwitchWithStop, OneCurveWithStop, OneCurve};
    }

    [[nodiscard]] static constexpr auto allNonEmpty() noexcept -> std::array<Stone, count - 1> {
        return {Crossing, CrossingWithStop, TwoCurves, SwitchA, SwitchB, SwitchC,
            CurveWithBounces, SwitchWithStop, OneCurveWithStop, OneCurve};
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return 1U;
    }

    void addToData(std::string &data) const noexcept {
        data.append(toString(Short));
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> Stone {
        if (data.size() != dataSize()) {
            throw Error("Stone: Invalid data size.");
        }
        switch (data.at(0)) {
            case '_': return {Empty};
            case 'A': return {Crossing};
            case 'B': return {CrossingWithStop};
            case 'C': return {TwoCurves};
            case 'D': return {SwitchA};
            case 'E': return {SwitchB};
            case 'F': return {SwitchC};
            case 'G': return {CurveWithBounces};
            case 'H': return {SwitchWithStop};
            case 'I': return {OneCurveWithStop};
            case 'J': return {OneCurve};
            default: return {Empty};
        }
    }

public: // conversion
    [[nodiscard]] auto toString(const Format format = Short) const noexcept -> std::string {
        if (format == Short) {
            switch (_type) {
            case Empty: return "_";
            case Crossing: return "A";
            case CrossingWithStop: return "B";
            case TwoCurves: return "C";
            case SwitchA: return "D";
            case SwitchB: return "E";
            case SwitchC: return "F";
            case CurveWithBounces: return "G";
            case SwitchWithStop: return "H";
            case OneCurveWithStop: return "I";
            case OneCurve: return "J";
            default: return " ";
            }
        }
        switch (_type) {
        case Empty: return "Empty";
        case Crossing: return "Crossing";
        case CrossingWithStop: return "CrossingWithStop";
        case TwoCurves: return "TwoCurves";
        case SwitchA: return "SwitchA";
        case SwitchB: return "SwitchB";
        case SwitchC: return "SwitchC";
        case CurveWithBounces: return "CurveWithBounces";
        case SwitchWithStop: return "SwitchWithStop";
        case OneCurveWithStop: return "OneCurveWithStop";
        case OneCurve: return "OneCurve";
        default: return {};
        }
    }

    [[nodiscard]] auto toDebugString() const noexcept -> std::string {
        std::stringstream result;
        result << "Stone(" << toString(Long) << ", ";
        result << "uniqueOrientations = {" << uniqueOrientations().toString() << "}, ";
        result << "connections = {";
        for (auto cp : Anchor::all()) {
            result << cp.toString() << "->" << thisWiring().connections.at(cp.value()).toString() << " ";
        }
        result << ")";
        return result.str();
    }


public: // wiring
    /// Access the stone wiring data.
    ///
    [[nodiscard]] static auto wiring() noexcept -> const Wiring&;

    /// Access the stone wiring data for this type.
    ///
    [[nodiscard]] auto thisWiring() const noexcept -> const StoneWiring& {
        return wiring().at(_type);
    }

private:
    Type _type{Empty};
};


static_assert(Serializable<Stone>);


using StoneList = std::vector<Stone>;
using StonePair = std::pair<Stone, Stone>;
using StonePairList = std::vector<StonePair>;
using StoneQuad = std::tuple<Stone, Stone, Stone, Stone>;
using StoneQuadList = std::vector<StoneQuad>;


template<>
struct std::hash<Stone> {
    auto operator()(const Stone &stone) const noexcept -> std::size_t {
        return std::hash<Stone::Type>{}(stone.type());
    }
};

