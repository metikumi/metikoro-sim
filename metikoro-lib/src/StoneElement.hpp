// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Anchor.hpp"
#include "Rotation.hpp"

#include <cstdint>
#include <vector>


//    Straight   Stop       Curve      Bounce
//    ┌──┬──┐    ┌──┬──┐    ┌──┬──┐    ┌──┬──┐
//    │  │  │    │  │  │    │  ╰╮ │    │  △  │
//    │  |  │    │  ○  │    │   ╰─┤    │     │
//    │  │  │    │     │    │     │    │     │
//    └──┴──┘    └─────┘    └─────┘    └─────┘


class StoneElement {
public:
    static constexpr auto count = 4;
    enum Type : uint8_t {
        Straight = 0,
        Stop,
        Curve,
        Bounce,
    };
    using Connection = std::pair<Anchor, Anchor>;

    constexpr StoneElement(Type type, Orientation orientation) noexcept
        : _type{type}, _orientation{orientation} {
    }

    [[nodiscard]] constexpr auto type() const noexcept -> Type { return _type; }
    [[nodiscard]] constexpr auto orientation() const noexcept -> Orientation { return _orientation; }
    [[nodiscard]] constexpr auto connection() const noexcept -> Connection {
        return {
            Anchor{Anchor::North}.rotated(_orientation.toRotation()),
            std::array<Anchor, count>{
                Anchor::South,
                Anchor::Stop,
                Anchor::East,
                Anchor::North,
            }[_type].rotated(_orientation.toRotation()),
        };
    }
    [[nodiscard]] constexpr auto hasStop() const noexcept -> bool {
        return _type == Stop;
    }

private:
    Type _type;
    Orientation _orientation;
};


