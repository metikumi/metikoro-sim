// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Rotation.hpp"


class RotationTest : public el::UnitTest {
public:
    Rotation rot;

    void testDefault() {
        rot = {};
        REQUIRE(rot.value() == Rotation::None);
    }

    void testValue() {
        rot = Rotation::None;
        REQUIRE(rot.value() == Rotation::None);
        REQUIRE(rot == Rotation::None);
        REQUIRE(rot == 0);
        rot = Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::Clockwise90);
        REQUIRE(rot == Rotation::Clockwise90);
        REQUIRE(rot == 1);
        rot = Rotation::Clockwise180;
        REQUIRE(rot.value() == Rotation::Clockwise180);
        REQUIRE(rot == Rotation::Clockwise180);
        REQUIRE(rot == 2);
        rot = Rotation::Clockwise270;
        REQUIRE(rot.value() == Rotation::Clockwise270);
        REQUIRE(rot == Rotation::Clockwise270);
        REQUIRE(rot == 3);
        rot = Rotation::CounterClockwise90;
        REQUIRE(rot.value() == Rotation::CounterClockwise90);
        REQUIRE(rot == Rotation::CounterClockwise90);
        REQUIRE(rot == -1);
        rot = Rotation::CounterClockwise180;
        REQUIRE(rot.value() == Rotation::CounterClockwise180);
        REQUIRE(rot == Rotation::CounterClockwise180);
        REQUIRE(rot == -2);
        rot = Rotation::CounterClockwise270;
        REQUIRE(rot.value() == Rotation::CounterClockwise270);
        REQUIRE(rot == Rotation::CounterClockwise270);
        REQUIRE(rot == -3);
    }

    void testMath() {
        rot = Rotation::None;
        rot += Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::Clockwise90);
        rot += Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::Clockwise180);
        rot += Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::Clockwise270);
        rot += Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::None);
        rot += Rotation::Clockwise180;
        REQUIRE(rot.value() == Rotation::Clockwise180);
        rot += Rotation::Clockwise270;
        REQUIRE(rot.value() == Rotation::Clockwise90);

        rot = Rotation::None;
        rot = rot + Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::Clockwise90);
        rot = rot + Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::Clockwise180);
        rot = rot + Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::Clockwise270);
        rot = rot + Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::None);
        rot = rot + Rotation{Rotation::Clockwise180};
        REQUIRE(rot.value() == Rotation::Clockwise180);
        rot = rot + Rotation{Rotation::Clockwise270};
        REQUIRE(rot.value() == Rotation::Clockwise90);

        rot = Rotation::None;
        rot -= Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::CounterClockwise90);
        rot -= Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::CounterClockwise180);
        rot -= Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::CounterClockwise270);
        rot -= Rotation::Clockwise90;
        REQUIRE(rot.value() == Rotation::None);
        rot -= Rotation::Clockwise180;
        REQUIRE(rot.value() == Rotation::CounterClockwise180);
        rot -= Rotation::Clockwise270;
        REQUIRE(rot.value() == Rotation::CounterClockwise90);

        rot = Rotation::None;
        rot = rot - Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::CounterClockwise90);
        rot = rot - Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::CounterClockwise180);
        rot = rot - Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::CounterClockwise270);
        rot = rot - Rotation{Rotation::Clockwise90};
        REQUIRE(rot.value() == Rotation::None);
        rot = rot - Rotation{Rotation::Clockwise180};
        REQUIRE(rot.value() == Rotation::CounterClockwise180);
        rot = rot - Rotation{Rotation::Clockwise270};
        REQUIRE(rot.value() == Rotation::CounterClockwise90);
    }

    void testStrings() {
        rot = Rotation::None;
        REQUIRE(rot.toString() == "0º CW");
        rot = Rotation::Clockwise90;
        REQUIRE(rot.toString() == "90º CW");
        rot = Rotation::Clockwise180;
        REQUIRE(rot.toString() == "180º CW");
        rot = Rotation::Clockwise270;
        REQUIRE(rot.toString() == "270º CW");
        rot = Rotation::CounterClockwise90;
        REQUIRE(rot.toString() == "90º CCW");
        rot = Rotation::CounterClockwise180;
        REQUIRE(rot.toString() == "180º CCW");
        rot = Rotation::CounterClockwise270;
        REQUIRE(rot.toString() == "270º CCW");
    }
};

