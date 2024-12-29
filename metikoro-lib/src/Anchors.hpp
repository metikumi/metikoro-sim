// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Anchor.hpp"


class Anchors {
public:
    Anchors() = default;

    template <typename... Ts>
    constexpr Anchors(Anchor first, Ts... anchors)
        : _points((first.flag() | ... | anchors.flag())) {}

    template <typename... Ts>
    constexpr Anchors(Anchor::Value first, Ts... values) noexcept
        : _points(((Anchor(first).flag()) | ... | (Anchor(values).flag()))) {}

public: // operators
    [[nodiscard]] constexpr auto operator==(const Anchors &other) const noexcept -> bool { return _points == other._points; }
    [[nodiscard]] constexpr auto operator!=(const Anchors &other) const noexcept -> bool { return _points != other._points; }

    auto operator|=(const Anchor::Value value) noexcept -> Anchors& { _points |= Anchor{value}.flag(); return *this; }
    auto operator|=(const Anchor &other) noexcept -> Anchors& { _points |= other.flag(); return *this; }
    auto operator|=(const Anchors &other) noexcept -> Anchors& { _points |= other._points; return *this; }
    [[nodiscard]] auto operator|(const Anchor::Value value) const noexcept -> Anchors { return Anchors{static_cast<uint8_t>(_points | Anchor{value}.flag())}; }
    [[nodiscard]] auto operator|(const Anchor &other) const noexcept -> Anchors { return Anchors{static_cast<uint8_t>(_points | other.flag())}; }
    [[nodiscard]] auto operator|(const Anchors &other) const noexcept -> Anchors { return Anchors{static_cast<uint8_t>(_points | other._points)}; }
    [[nodiscard]] friend auto operator|(const Anchor::Value lhs, const Anchor::Value rhs) noexcept -> Anchors { return Anchors{lhs} | Anchor{rhs}; }
    [[nodiscard]] friend auto operator|(const Anchor &lhs, const Anchors &rhs) noexcept -> Anchors { return Anchors{lhs} | rhs; }
    [[nodiscard]] friend auto operator|(const Anchor &lhs, const Anchor &rhs) noexcept -> Anchors { return Anchors{lhs} | rhs; }

public: // accessors
    [[nodiscard]] constexpr auto empty() const noexcept { return _points == 0; }
    [[nodiscard]] constexpr auto contains(const Anchor connection) const noexcept -> bool { return (_points & connection.flag()) != 0; }
    [[nodiscard]] constexpr auto first() const noexcept -> Anchor {
        for (uint8_t i = 0; i < Anchor::count; ++i) {
            if ((_points & (1U << i)) != 0) {
                return Anchor{static_cast<Anchor::Value>(i)};
            }
        }
        return {};
    }

public: // conversion
    [[nodiscard]] constexpr auto toVector() const noexcept -> std::vector<Anchor::Value> {
        std::vector<Anchor::Value> result;
        for (uint8_t i = 0; i < Anchor::count; ++i) {
            if ((_points & (1U << i)) != 0) {
                result.push_back(static_cast<Anchor::Value>(i));
            }
        }
        return result;
    }

    [[nodiscard]] constexpr auto rotated(const Rotation rotation) const noexcept -> Anchors {
        const uint8_t sidePart = _points & Anchor::sideMask;
        const uint8_t shiftCount = rotation.wrapToClockwise().value() & 0x03U;
        const auto rotatedLower = static_cast<uint8_t>(
            (
                (sidePart << shiftCount) |
                (sidePart >> (Anchor::sideCount - shiftCount))
            ) & Anchor::sideMask);
        const uint8_t centerPart = _points & Anchor::centerMask;
        return Anchors{static_cast<uint8_t>(rotatedLower | centerPart)};
    }

    /// Rotated for a given orientation.
    ///
    [[nodiscard]] constexpr auto rotated(const Orientation orientation) const noexcept -> Anchors {
        return rotated(orientation.toRotation());
    }

    /// Return the anchor, normalized for the given stone orientation.
    ///
    [[nodiscard]] constexpr auto normalized(const Orientation orientation) const noexcept -> Anchors {
        return rotated(orientation.toRotation().reversed());
    }

    [[nodiscard]] auto toString() const noexcept -> std::string {
        std::string result;
        for (const auto cp : Anchor::all()) {
            if (contains(cp)) {
                result += cp.toString();
            }
        }
        return result;
    }

public: // modifiers
    void remove(const Anchor connectionPoint) noexcept {
        _points &= ~connectionPoint.flag();
    }

private:
    explicit constexpr Anchors(const uint8_t sides) : _points(sides) {};

private:
    uint8_t _points{0};
};

