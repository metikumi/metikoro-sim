// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "StoneWiring.hpp"


class StoneWiringTest : public el::UnitTest {
public:
    StoneWiring wiring;

    void testEmpty() {
        wiring = {};

        REQUIRE_FALSE(wiring.hasStop());
        REQUIRE(wiring.connections[Anchor::North].empty());
        REQUIRE(wiring.connections[Anchor::East].empty());
        REQUIRE(wiring.connections[Anchor::South].empty());
        REQUIRE(wiring.connections[Anchor::West].empty());
        REQUIRE(wiring.connections[Anchor::Stop].empty());
        REQUIRE(wiring.uniqueOrientations.toVector() == std::vector<Orientation>{Orientation::North});

        REQUIRE(wiring.isEqual(Orientation::North, Orientation::North));
        REQUIRE(wiring.isEqual(Orientation::North, Orientation::East));
        REQUIRE(wiring.isEqual(Orientation::North, Orientation::South));
        REQUIRE(wiring.isEqual(Orientation::North, Orientation::West));
        REQUIRE(wiring.isEqual(Orientation::East, Orientation::North));
        REQUIRE(wiring.isEqual(Orientation::East, Orientation::East));
        REQUIRE(wiring.isEqual(Orientation::East, Orientation::South));
        REQUIRE(wiring.isEqual(Orientation::East, Orientation::West));
        REQUIRE(wiring.isEqual(Orientation::South, Orientation::North));
        REQUIRE(wiring.isEqual(Orientation::South, Orientation::East));
        REQUIRE(wiring.isEqual(Orientation::South, Orientation::South));
        REQUIRE(wiring.isEqual(Orientation::South, Orientation::West));
        REQUIRE(wiring.isEqual(Orientation::West, Orientation::North));
        REQUIRE(wiring.isEqual(Orientation::West, Orientation::East));
        REQUIRE(wiring.isEqual(Orientation::West, Orientation::South));
        REQUIRE(wiring.isEqual(Orientation::West, Orientation::West));
    }

    void testStraight() {
        wiring = {};
        wiring |= StoneElement{StoneElement::Straight, Orientation::North};
        REQUIRE(wiring.connections[Anchor::North].toVector() == std::vector{Anchor::South});
        REQUIRE(wiring.connections[Anchor::East].empty());
        REQUIRE(wiring.connections[Anchor::South].toVector() == std::vector{Anchor::North});
        REQUIRE(wiring.connections[Anchor::West].empty());
        REQUIRE(wiring.connections[Anchor::Stop].empty());
        REQUIRE(wiring.uniqueOrientations.toVector() == std::vector<Orientation>{Orientation::North, Orientation::East});
    }

    void testRotatedConnections() {
        wiring = {};
        wiring |= StoneElement{StoneElement::Straight, Orientation::North};
        auto newConnections = wiring.rotatedConnections(Rotation::Clockwise90);
        REQUIRE(newConnections[Anchor::North].empty());
        REQUIRE(newConnections[Anchor::East].toVector() == std::vector{Anchor::West});
        REQUIRE(newConnections[Anchor::South].empty());
        REQUIRE(newConnections[Anchor::West].toVector() == std::vector{Anchor::East});
        REQUIRE(newConnections[Anchor::Stop].empty());
    }

    void testCross() {
        wiring = {};
        wiring |= StoneElement{StoneElement::Straight, Orientation::North};
        wiring |= StoneElement{StoneElement::Straight, Orientation::East};
        REQUIRE(wiring.connections[Anchor::North].toVector() == std::vector{Anchor::South});
        REQUIRE(wiring.connections[Anchor::East].toVector() == std::vector{Anchor::West});
        REQUIRE(wiring.connections[Anchor::South].toVector() == std::vector{Anchor::North});
        REQUIRE(wiring.connections[Anchor::West].toVector() == std::vector{Anchor::East});
        REQUIRE(wiring.connections[Anchor::Stop].empty());
        REQUIRE(wiring.uniqueOrientations.toVector() == std::vector<Orientation>{Orientation::North});
    }

    void testCurve() {
        wiring = {};
        wiring |= StoneElement{StoneElement::Curve, Orientation::North};
        REQUIRE(wiring.connections[Anchor::North].toVector() == std::vector{Anchor::East});
        REQUIRE(wiring.connections[Anchor::East].toVector() == std::vector{Anchor::North});
        REQUIRE(wiring.connections[Anchor::South].empty());
        REQUIRE(wiring.connections[Anchor::West].empty());
        REQUIRE(wiring.connections[Anchor::Stop].empty());
        REQUIRE(wiring.uniqueOrientations.toVector() == std::vector<Orientation>{Orientation::North, Orientation::East, Orientation::South, Orientation::West});
    }

    void testOneStop() {
        wiring = {};
        wiring |= StoneElement{StoneElement::Stop, Orientation::North};
        REQUIRE(wiring.connections[Anchor::North].toVector() == std::vector{Anchor::Stop});
        REQUIRE(wiring.connections[Anchor::East].empty());
        REQUIRE(wiring.connections[Anchor::South].empty());
        REQUIRE(wiring.connections[Anchor::West].empty());
        REQUIRE(wiring.connections[Anchor::Stop].toVector() == std::vector{Anchor::North});
        REQUIRE(wiring.uniqueOrientations.toVector() == std::vector<Orientation>{Orientation::North, Orientation::East, Orientation::South, Orientation::West});
    }

    void testCrossingWithStop() {
        wiring = {};
        wiring |= StoneElement{StoneElement::Stop, Orientation::North};
        wiring |= StoneElement{StoneElement::Stop, Orientation::East};
        wiring |= StoneElement{StoneElement::Stop, Orientation::South};
        wiring |= StoneElement{StoneElement::Stop, Orientation::West};
        REQUIRE(wiring.connections[Anchor::North].toVector() == std::vector{Anchor::Stop});
        REQUIRE(wiring.connections[Anchor::East].toVector() == std::vector{Anchor::Stop});
        REQUIRE(wiring.connections[Anchor::South].toVector() == std::vector{Anchor::Stop});
        REQUIRE(wiring.connections[Anchor::West].toVector() == std::vector{Anchor::Stop});
        REQUIRE(wiring.connections[Anchor::Stop].toVector() == std::vector{Anchor::North, Anchor::East, Anchor::South, Anchor::West});
        REQUIRE(wiring.uniqueOrientations.toVector() == std::vector<Orientation>{Orientation::North});
    }
};

