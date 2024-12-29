// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Anchor.hpp"


class AnchorTest : public el::UnitTest {
public:
    Anchor cp;

    void testDefault() {
        cp = {};
        REQUIRE(cp.value() == Anchor::North);
    }

    void testValue() {
        cp = Anchor::North;
        REQUIRE(cp.value() == Anchor::North);
        REQUIRE(cp.flag() == 1U);
        cp = Anchor::East;
        REQUIRE(cp.value() == Anchor::East);
        REQUIRE(cp.flag() == 2U);
        cp = Anchor::South;
        REQUIRE(cp.value() == Anchor::South);
        REQUIRE(cp.flag() == 4U);
        cp = Anchor::West;
        REQUIRE(cp.value() == Anchor::West);
        REQUIRE(cp.flag() == 8U);
        cp = Anchor::Stop;
        REQUIRE(cp.value() == Anchor::Stop);
        REQUIRE(cp.flag() == 16U);
    }

    void testEqual() {
        cp = Anchor::North;
        REQUIRE(cp == Anchor::North);
        REQUIRE(cp != Anchor::East);
        REQUIRE(cp == Anchor{Anchor::North});
    }

    void testRotation() {
        cp = Anchor::North;
        cp = cp.rotated(Rotation::None);
        REQUIRE(cp.value() == Anchor::North);
        cp = cp.rotated(Rotation::Clockwise90);
        REQUIRE(cp.value() == Anchor::East);
        cp = Anchor::North;
        cp = cp.rotated(Rotation::Clockwise180);
        REQUIRE(cp.value() == Anchor::South);
        cp = Anchor::North;
        cp = cp.rotated(Rotation::Clockwise270);
        REQUIRE(cp.value() == Anchor::West);

        cp = Anchor::East;
        cp = cp.rotated(Rotation::None);
        REQUIRE(cp.value() == Anchor::East);
        cp = cp.rotated(Rotation::Clockwise90);
        REQUIRE(cp.value() == Anchor::South);
        cp = Anchor::East;
        cp = cp.rotated(Rotation::Clockwise180);
        REQUIRE(cp.value() == Anchor::West);
        cp = Anchor::East;
        cp = cp.rotated(Rotation::Clockwise270);
        REQUIRE(cp.value() == Anchor::North);

        cp = Anchor::South;
        cp = cp.rotated(Rotation::None);
        REQUIRE(cp.value() == Anchor::South);
        cp = Anchor::South;
        cp = cp.rotated(Rotation::Clockwise90);
        REQUIRE(cp.value() == Anchor::West);
        cp = Anchor::South;
        cp = cp.rotated(Rotation::Clockwise180);
        REQUIRE(cp.value() == Anchor::North);
        cp = Anchor::South;
        cp = cp.rotated(Rotation::Clockwise270);
        REQUIRE(cp.value() == Anchor::East);

        cp = Anchor::West;
        cp = cp.rotated(Rotation::None);
        REQUIRE(cp.value() == Anchor::West);
        cp = Anchor::West;
        cp = cp.rotated(Rotation::Clockwise90);
        REQUIRE(cp.value() == Anchor::North);
        cp = Anchor::West;
        cp = cp.rotated(Rotation::Clockwise180);
        REQUIRE(cp.value() == Anchor::East);
        cp = Anchor::West;
        cp = cp.rotated(Rotation::Clockwise270);
        REQUIRE(cp.value() == Anchor::South);

        cp = Anchor::Stop;
        cp = cp.rotated(Rotation::None);
        REQUIRE(cp.value() == Anchor::Stop);
        cp = Anchor::Stop;
        cp = cp.rotated(Rotation::Clockwise90);
        REQUIRE(cp.value() == Anchor::Stop);
        cp = Anchor::Stop;
        cp = cp.rotated(Rotation::Clockwise180);
        REQUIRE(cp.value() == Anchor::Stop);
        cp = Anchor::Stop;
        cp = cp.rotated(Rotation::Clockwise270);
        REQUIRE(cp.value() == Anchor::Stop);
    }

    void testAll() {
        auto all = Anchor::all();
        REQUIRE(all.size() == 5);
        REQUIRE(all[0].value() == Anchor::North);
        REQUIRE(all[1].value() == Anchor::East);
        REQUIRE(all[2].value() == Anchor::South);
        REQUIRE(all[3].value() == Anchor::West);
        REQUIRE(all[4].value() == Anchor::Stop);
    }

    void testToString() {
        cp = Anchor::North;
        REQUIRE(cp.toString() == "N");
        cp = Anchor::East;
        REQUIRE(cp.toString() == "E");
        cp = Anchor::South;
        REQUIRE(cp.toString() == "S");
        cp = Anchor::West;
        REQUIRE(cp.toString() == "W");
        cp = Anchor::Stop;
        REQUIRE(cp.toString() == "O");
    }
};

