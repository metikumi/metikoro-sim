// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Orientation.hpp"


class Orientations {
public:
    Orientations() = default;
    template <typename... Ts>
    constexpr Orientations(Orientation first, Ts... orientations) noexcept
        : _orientations((first.flag() | ... | orientations.flag())) {}
    template <typename... Ts>
    constexpr Orientations(Orientation::Value first, Ts... values) noexcept
        : _orientations(((Orientation(first).flag()) | ... | (Orientation(values).flag()))) {}

    [[nodiscard]] auto operator==(const Orientations &other) const noexcept -> bool = default;

    auto operator|(const Orientation &orientation) const noexcept -> Orientations {
        return Orientations{static_cast<uint8_t>(_orientations | orientation.flag())};
    }
    auto operator|(const Orientations &orientations) const noexcept -> Orientations {
        return Orientations{static_cast<uint8_t>(_orientations | orientations._orientations)};
    }
    auto operator|=(const Orientation &orientation) noexcept -> Orientations& { _orientations |= orientation.flag(); return *this; }
    auto operator|=(const Orientations &orientations) noexcept -> Orientations& { _orientations |= orientations._orientations; return *this; }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool { return _orientations == 0; }
    [[nodiscard]] constexpr auto contains(Orientation orientation) const noexcept -> bool { return (_orientations & orientation.flag()) != 0; }
    [[nodiscard]] constexpr auto toVector() const noexcept -> std::vector<Orientation> {
        std::vector<Orientation> result;
        for (uint8_t i = 0; i < Orientation::count; ++i) {
            if ((_orientations & (1U << i)) != 0) {
                result.emplace_back(static_cast<Orientation::Value>(i));
            }
        }
        return result;
    }
    [[nodiscard]] auto toString() const noexcept -> std::string {
        std::string result;
        for (const auto orientation : Orientation::all()) {
            if (contains(orientation)) {
                result += orientation.toArrow();
            }
        }
        return result;
    }

private:
    explicit constexpr Orientations(const uint8_t orientations) noexcept : _orientations{orientations} {};

private:
    uint8_t _orientations{0};
};

