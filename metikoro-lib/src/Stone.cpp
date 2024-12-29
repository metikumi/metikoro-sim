// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#include "Stone.hpp"


auto Stone::wiring() noexcept -> const Wiring& {
    static const Wiring result = {
        //Empty = 0,
        StoneWiring{},
        // Crossing, // A
        StoneElement{StoneElement::Straight, Orientation::North} |
        StoneElement{StoneElement::Straight, Orientation::East},
        // CrossingWithStop, // B
        StoneElement{StoneElement::Stop, Orientation::North} |
        StoneElement{StoneElement::Stop, Orientation::East} |
        StoneElement{StoneElement::Stop, Orientation::South} |
        StoneElement{StoneElement::Stop, Orientation::West},
        // TwoCurves, // C
        StoneElement{StoneElement::Curve, Orientation::North} |
        StoneElement{StoneElement::Curve, Orientation::South},
        // SwitchA, // D
        StoneElement{StoneElement::Straight, Orientation::North} |
        StoneElement{StoneElement::Curve, Orientation::North} |
        StoneElement{StoneElement::Curve, Orientation::South},
        // SwitchB, // E
        StoneElement{StoneElement::Straight, Orientation::North} |
        StoneElement{StoneElement::Curve, Orientation::North} |
        StoneElement{StoneElement::Curve, Orientation::West},
        // SwitchC, // F
        StoneElement{StoneElement::Curve, Orientation::North} |
        StoneElement{StoneElement::Curve, Orientation::East} |
        StoneElement{StoneElement::Curve, Orientation::South} |
        StoneElement{StoneElement::Curve, Orientation::West},
        // CurveWithBounces, // G
        StoneElement{StoneElement::Curve, Orientation::West} |
        StoneElement{StoneElement::Bounce, Orientation::East} |
        StoneElement{StoneElement::Bounce, Orientation::South},
        // SwitchWithStop, // H
        StoneElement{StoneElement::Stop, Orientation::North} |
        StoneElement{StoneElement::Stop, Orientation::East} |
        StoneElement{StoneElement::Stop, Orientation::South},
        // OneCurveWithStop, // I
        StoneElement{StoneElement::Stop, Orientation::North} |
        StoneElement{StoneElement::Stop, Orientation::East},
        // OneCurve, // J
        StoneWiring{StoneElement{StoneElement::Curve, Orientation::North}},
    };
    return result;
}

