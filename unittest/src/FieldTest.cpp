// Copyright (c) 2024-2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Field.hpp"


class FieldTest : public el::UnitTest {
public:
    Field field;

    void testDefault() {
        field = {};
        REQUIRE(field == Field{})
        REQUIRE(field.stone() == Stone::Empty);
        REQUIRE(field.orientation() == Orientation::North);
        REQUIRE(field.hasKoLock() == false);
        REQUIRE(field.koLock() == 0);
        REQUIRE(field.type() == Stone::Empty);
        REQUIRE(field.empty() == true);
        REQUIRE(field.hasStop() == false);
        REQUIRE(field.connectionsFrom(Anchor::North).empty());
        REQUIRE(field.connectionsFrom(Anchor::East).empty());
        REQUIRE(field.connectionsFrom(Anchor::South).empty());
        REQUIRE(field.connectionsFrom(Anchor::West).empty());
        REQUIRE(field.connectionsFrom(Anchor::Stop).empty());
        REQUIRE(field.uniqueOrientations() == Orientations{Orientation::North});
        REQUIRE(field.canRotate() == false);
        REQUIRE(field.isValidChange(Stone::Crossing, Orientation::North) == false);
        REQUIRE(field.toBoardString() == "  ");
    }

    void testStone() {
        field = {};
        field.setStone(Stone::Crossing, Orientation::North);
        REQUIRE(field != Field{});
        REQUIRE(field.stone() == Stone::Crossing);
        REQUIRE(field.orientation() == Orientation::North);
        REQUIRE(field.hasKoLock() == false);
        REQUIRE(field.koLock() == 0);
        REQUIRE(field.type() == Stone::Crossing);
        REQUIRE(field.empty() == false);
        REQUIRE(field.hasStop() == false);
        REQUIRE(field.connectionsFrom(Anchor::North) == Anchors{Anchor::South});
        REQUIRE(field.connectionsFrom(Anchor::East) == Anchors{Anchor::West});
        REQUIRE(field.connectionsFrom(Anchor::South) == Anchors{Anchor::North});
        REQUIRE(field.connectionsFrom(Anchor::West) == Anchors{Anchor::East});
        REQUIRE(field.connectionsFrom(Anchor::Stop).empty());
        REQUIRE(field.uniqueOrientations() == Orientations{Orientation::North});
        REQUIRE(field.canRotate() == false);
        REQUIRE(field.isValidChange(Stone::Crossing, Orientation::North) == false);
        REQUIRE(field.isValidChange(Stone::TwoCurves, Orientation::North) == true);
    }

    void testOrientation() {
        field = {};
        field.setStone(Stone::OneCurve, Orientation::North);
        REQUIRE(field.connectionsFrom(Anchor::North) == Anchors{Anchor::East});
        REQUIRE(field.connectionsFrom(Anchor::East) == Anchors{Anchor::North});
        REQUIRE(field.connectionsFrom(Anchor::South).empty());
        REQUIRE(field.connectionsFrom(Anchor::West).empty());
        REQUIRE(field.connectionsFrom(Anchor::Stop).empty());
        field.setOrientation(Orientation::East);
        REQUIRE(field.connectionsFrom(Anchor::North).empty());
        REQUIRE(field.connectionsFrom(Anchor::East) == Anchors{Anchor::South});
        REQUIRE(field.connectionsFrom(Anchor::South) == Anchors{Anchor::East});
        REQUIRE(field.connectionsFrom(Anchor::West).empty());
        REQUIRE(field.connectionsFrom(Anchor::Stop).empty());
        field.setOrientation(Orientation::South);
        REQUIRE(field.connectionsFrom(Anchor::North).empty());
        REQUIRE(field.connectionsFrom(Anchor::East).empty());
        REQUIRE(field.connectionsFrom(Anchor::South) == Anchors{Anchor::West});
        REQUIRE(field.connectionsFrom(Anchor::West) == Anchors{Anchor::South});
        REQUIRE(field.connectionsFrom(Anchor::Stop).empty());
        field.setOrientation(Orientation::West);
        REQUIRE(field.connectionsFrom(Anchor::North) == Anchors{Anchor::West});
        REQUIRE(field.connectionsFrom(Anchor::East).empty());
        REQUIRE(field.connectionsFrom(Anchor::South).empty());
        REQUIRE(field.connectionsFrom(Anchor::West) == Anchors{Anchor::North});
        REQUIRE(field.connectionsFrom(Anchor::Stop).empty());
    }

    void testKoLock() {
        field = {};
        field.setStone(Stone::TwoCurves, Orientation::North);
        REQUIRE(field.hasKoLock() == false);
        REQUIRE(field.koLock() == 0);
        field.setKoLock(3);
        REQUIRE(field.hasKoLock() == true);
        REQUIRE(field.koLock() == 3);
        field.nextTurn();
        REQUIRE(field.hasKoLock() == true);
        REQUIRE(field.koLock() == 2);
        field.nextTurn();
        REQUIRE(field.hasKoLock() == true);
        REQUIRE(field.koLock() == 1);
        field.nextTurn();
        REQUIRE(field.hasKoLock() == false);
        REQUIRE(field.koLock() == 0);
        field.nextTurn();
        REQUIRE(field.hasKoLock() == false);
        REQUIRE(field.koLock() == 0);
        field.nextTurn();
        REQUIRE(field.hasKoLock() == false);
        REQUIRE(field.koLock() == 0);
    }

    void testRotation() {
        field = {};
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.stone() == Stone::Empty);
        REQUIRE(field.orientation() == Orientation::North);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::North);

        field.setStone(Stone::Crossing, Orientation::North);
        REQUIRE(field.stone() == Stone::Crossing);
        REQUIRE(field.orientation() == Orientation::North);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.stone() == Stone::Crossing);
        REQUIRE(field.orientation() == Orientation::North);
        field = field.rotated(Rotation::Clockwise180);
        REQUIRE(field.stone() == Stone::Crossing);
        REQUIRE(field.orientation() == Orientation::North);
        field = field.rotated(Rotation::Clockwise270);
        REQUIRE(field.stone() == Stone::Crossing);
        REQUIRE(field.orientation() == Orientation::North);

        field = Field(Stone::Crossing, Orientation::West, 0);
        REQUIRE(field.stone() == Stone::Crossing);
        REQUIRE(field.orientation() == Orientation::North);
        REQUIRE(field.koLock() == 0);

        field = Field(Stone::OneCurveWithStop, Orientation::West, 0);
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::West);
        REQUIRE(field.koLock() == 0);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::North);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::East);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::South);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::West);

        field = Field(Stone::TwoCurves, Orientation::West, 0);
        REQUIRE(field.stone() == Stone::TwoCurves);
        REQUIRE(field.orientation() == Orientation::East);
        REQUIRE(field.koLock() == 0);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::North);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::East);
        field = field.rotated(Rotation::Clockwise90);
        REQUIRE(field.orientation() == Orientation::North);
    }
};

