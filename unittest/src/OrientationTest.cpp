// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Orientation.hpp"


class OrientationTest : public el::UnitTest {
public:
    Orientation ori;

    void testDefault() {
        ori = {};
        REQUIRE(ori == Orientation::North);
    }

    void testValue() {
        ori = Orientation::North;
        REQUIRE(ori == Orientation::North);
        REQUIRE(ori.value() == Orientation::North);
        REQUIRE(ori.flag() == 1U);
        ori = Orientation::East;
        REQUIRE(ori == Orientation::East);
        REQUIRE(ori.value() == Orientation::East);
        REQUIRE(ori.flag() == 2U);
        ori = Orientation::South;
        REQUIRE(ori == Orientation::South);
        REQUIRE(ori.value() == Orientation::South);
        REQUIRE(ori.flag() == 4U);
        ori = Orientation::West;
        REQUIRE(ori == Orientation::West);
        REQUIRE(ori.value() == Orientation::West);
        REQUIRE(ori.flag() == 8U);
    }

    void testToRotation() {
        ori = Orientation::North;
        REQUIRE(ori.toRotation() == Rotation::None);
        ori = Orientation::East;
        REQUIRE(ori.toRotation() == Rotation::Clockwise90);
        ori = Orientation::South;
        REQUIRE(ori.toRotation() == Rotation::Clockwise180);
        ori = Orientation::West;
        REQUIRE(ori.toRotation() == Rotation::Clockwise270);
    }

    void testRotateOrientation() {
        ori = Orientation::North;
        REQUIRE((ori + Rotation::None) == Orientation::North);
        REQUIRE((ori + Rotation::Clockwise90) == Orientation::East);
        REQUIRE((ori + Rotation::Clockwise180) == Orientation::South);
        REQUIRE((ori + Rotation::Clockwise270) == Orientation::West);
        REQUIRE((ori + Rotation::CounterClockwise90) == Orientation::West);
        REQUIRE((ori + Rotation::CounterClockwise180) == Orientation::South);
        REQUIRE((ori + Rotation::CounterClockwise270) == Orientation::East);
        ori = Orientation::East;
        REQUIRE((ori + Rotation::None) == Orientation::East);
        REQUIRE((ori + Rotation::Clockwise90) == Orientation::South);
        REQUIRE((ori + Rotation::Clockwise180) == Orientation::West);
        REQUIRE((ori + Rotation::Clockwise270) == Orientation::North);
        REQUIRE((ori + Rotation::CounterClockwise90) == Orientation::North);
        REQUIRE((ori + Rotation::CounterClockwise180) == Orientation::West);
        REQUIRE((ori + Rotation::CounterClockwise270) == Orientation::South);
        ori = Orientation::South;
        REQUIRE((ori + Rotation::None) == Orientation::South);
        REQUIRE((ori + Rotation::Clockwise90) == Orientation::West);
        REQUIRE((ori + Rotation::Clockwise180) == Orientation::North);
        REQUIRE((ori + Rotation::Clockwise270) == Orientation::East);
        REQUIRE((ori + Rotation::CounterClockwise90) == Orientation::East);
        REQUIRE((ori + Rotation::CounterClockwise180) == Orientation::North);
        REQUIRE((ori + Rotation::CounterClockwise270) == Orientation::West);
        ori = Orientation::West;
        REQUIRE((ori + Rotation::None) == Orientation::West);
        REQUIRE((ori + Rotation::Clockwise90) == Orientation::North);
        REQUIRE((ori + Rotation::Clockwise180) == Orientation::East);
        REQUIRE((ori + Rotation::Clockwise270) == Orientation::South);
        REQUIRE((ori + Rotation::CounterClockwise90) == Orientation::South);
        REQUIRE((ori + Rotation::CounterClockwise180) == Orientation::East);
        REQUIRE((ori + Rotation::CounterClockwise270) == Orientation::North);

        ori = Orientation::North;
        REQUIRE((ori - Rotation::None) == Orientation::North);
        REQUIRE((ori - Rotation::Clockwise90) == Orientation::West);
        REQUIRE((ori - Rotation::Clockwise180) == Orientation::South);
        REQUIRE((ori - Rotation::Clockwise270) == Orientation::East);
        REQUIRE((ori - Rotation::CounterClockwise90) == Orientation::East);
        REQUIRE((ori - Rotation::CounterClockwise180) == Orientation::South);
        REQUIRE((ori - Rotation::CounterClockwise270) == Orientation::West);
        ori = Orientation::East;
        REQUIRE((ori - Rotation::None) == Orientation::East);
        REQUIRE((ori - Rotation::Clockwise90) == Orientation::North);
        REQUIRE((ori - Rotation::Clockwise180) == Orientation::West);
        REQUIRE((ori - Rotation::Clockwise270) == Orientation::South);
        REQUIRE((ori - Rotation::CounterClockwise90) == Orientation::South);
        REQUIRE((ori - Rotation::CounterClockwise180) == Orientation::West);
        REQUIRE((ori - Rotation::CounterClockwise270) == Orientation::North);
    }
};

