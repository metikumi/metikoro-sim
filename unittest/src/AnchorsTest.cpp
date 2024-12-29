// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Anchors.hpp"


class AnchorsTest : public el::UnitTest {
public:
    Anchors cps;

    void testDefault() {
        cps = {};
        REQUIRE(cps.empty());
        REQUIRE(cps.toVector().empty());
    }

    void testSingle() {
        cps = {};
        cps |= Anchor::North;
        REQUIRE(cps.contains(Anchor::North));
        REQUIRE_FALSE(cps.contains(Anchor::East));
        REQUIRE_FALSE(cps.contains(Anchor::South));
        REQUIRE_FALSE(cps.contains(Anchor::West));
        REQUIRE_FALSE(cps.contains(Anchor::Stop));
        REQUIRE(cps.toVector().size() == 1);
        REQUIRE(cps.toVector()[0] == Anchor::North);

        cps = {Anchor::East};
        REQUIRE_FALSE(cps.contains(Anchor::North));
        REQUIRE(cps.contains(Anchor::East));
        REQUIRE_FALSE(cps.contains(Anchor::South));
        REQUIRE_FALSE(cps.contains(Anchor::West));
        REQUIRE_FALSE(cps.contains(Anchor::Stop));
        REQUIRE(cps.toVector().size() == 1);
        REQUIRE(cps.toVector()[0] == Anchor::East);

        cps = {};
        cps = cps | Anchor{Anchor::South};
        REQUIRE_FALSE(cps.contains(Anchor::North));
        REQUIRE_FALSE(cps.contains(Anchor::East));
        REQUIRE(cps.contains(Anchor::South));
        REQUIRE_FALSE(cps.contains(Anchor::West));
        REQUIRE_FALSE(cps.contains(Anchor::Stop));
        REQUIRE(cps.toVector().size() == 1);
        REQUIRE(cps.toVector()[0] == Anchor::South);

        cps = {};
        cps = cps | Anchor::West;
        REQUIRE_FALSE(cps.contains(Anchor::North));
        REQUIRE_FALSE(cps.contains(Anchor::East));
        REQUIRE_FALSE(cps.contains(Anchor::South));
        REQUIRE(cps.contains(Anchor::West));
        REQUIRE_FALSE(cps.contains(Anchor::Stop));
        REQUIRE(cps.toVector().size() == 1);
        REQUIRE(cps.toVector()[0] == Anchor::West);

        cps = Anchor::Stop;
        REQUIRE_FALSE(cps.contains(Anchor::North));
        REQUIRE_FALSE(cps.contains(Anchor::East));
        REQUIRE_FALSE(cps.contains(Anchor::South));
        REQUIRE_FALSE(cps.contains(Anchor::West));
        REQUIRE(cps.contains(Anchor::Stop));
        REQUIRE(cps.toVector().size() == 1);
        REQUIRE(cps.toVector()[0] == Anchor::Stop);
    }

    void testCombinations() {
        cps = {Anchor::North, Anchor::East};
        REQUIRE(cps.contains(Anchor::North));
        REQUIRE(cps.contains(Anchor::East));
        REQUIRE_FALSE(cps.contains(Anchor::South));
        REQUIRE_FALSE(cps.contains(Anchor::West));
        REQUIRE_FALSE(cps.contains(Anchor::Stop));
        REQUIRE(cps.toVector().size() == 2);
        REQUIRE(cps.toVector()[0] == Anchor::North);
        REQUIRE(cps.toVector()[1] == Anchor::East);

        cps = {Anchor::Stop, Anchor::West};
        REQUIRE_FALSE(cps.contains(Anchor::North));
        REQUIRE_FALSE(cps.contains(Anchor::East));
        REQUIRE_FALSE(cps.contains(Anchor::South));
        REQUIRE(cps.contains(Anchor::West));
        REQUIRE(cps.contains(Anchor::Stop));
        REQUIRE(cps.toVector().size() == 2);
        REQUIRE(cps.toVector()[0] == Anchor::West);
        REQUIRE(cps.toVector()[1] == Anchor::Stop);
    }

    void testSingleRotation() {
        cps = Anchor::North;
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::East});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::South});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::West});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::North});
        cps = cps.rotated(Rotation::Clockwise180);
        REQUIRE(cps.toVector() == std::vector{Anchor::South});
        cps = cps.rotated(Rotation::Clockwise180);
        REQUIRE(cps.toVector() == std::vector{Anchor::North});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::East});
        cps = cps.rotated(Rotation::Clockwise180);
        REQUIRE(cps.toVector() == std::vector{Anchor::West});
        cps = cps.rotated(Rotation::Clockwise270);
        REQUIRE(cps.toVector() == std::vector{Anchor::South});
        cps = cps.rotated(Rotation::Clockwise270);
        REQUIRE(cps.toVector() == std::vector{Anchor::East});
        cps = cps.rotated(Rotation::Clockwise270);
        REQUIRE(cps.toVector() == std::vector{Anchor::North});
        cps = cps.rotated(Rotation::Clockwise270);
        REQUIRE(cps.toVector() == std::vector{Anchor::West});
        cps = Anchor::Stop;
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise180);
        REQUIRE(cps.toVector() == std::vector{Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise270);
        REQUIRE(cps.toVector() == std::vector{Anchor::Stop});
    }

    void testCombinedRotation() {
        cps = {Anchor::North, Anchor::Stop};
        REQUIRE(cps.toVector() == std::vector{Anchor::North, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::East, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::South, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::West, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::North, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise180);
        REQUIRE(cps.toVector() == std::vector{Anchor::South, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise180);
        REQUIRE(cps.toVector() == std::vector{Anchor::North, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::East, Anchor::Stop});

        cps = {Anchor::North, Anchor::West, Anchor::Stop};
        REQUIRE(cps.toVector() == std::vector{Anchor::North, Anchor::West, Anchor::Stop});
        cps = cps.rotated(Rotation::Clockwise90);
        REQUIRE(cps.toVector() == std::vector{Anchor::North, Anchor::East, Anchor::Stop});
    }

    void testToString() {
        cps = {};
        REQUIRE(cps.toString() == "");
        cps = {Anchor::North};
        REQUIRE(cps.toString() == "N");
        cps = {Anchor::North, Anchor::East};
        REQUIRE(cps.toString() == "NE");
        cps = {Anchor::North, Anchor::East, Anchor::South};
        REQUIRE(cps.toString() == "NES");
        cps = {Anchor::North, Anchor::East, Anchor::South, Anchor::West};
        REQUIRE(cps.toString() == "NESW");
        cps = {Anchor::North, Anchor::East, Anchor::South, Anchor::West, Anchor::Stop};
        REQUIRE(cps.toString() == "NESWO");
    }
};
