// Copyright (c) 2024-2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Board.hpp"


class BoardTest : public el::UnitTest {
public:
    Board board;

    // Just a few basic tests, added after an unexpected initialization error.
    void testInitial() {
        board = {};
        auto field = board.field(Position{0,0});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::East);
        REQUIRE(Board::isHouse(Position{0, 0}));
        field = board.field(Position{9, 0});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::South);
        REQUIRE(Board::isHouse(Position{9, 0}));
        field = board.field(Position{9, 9});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::West);
        REQUIRE(Board::isHouse(Position{9, 9}));
        field = board.field(Position{0, 9});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::North);
        REQUIRE(Board::isHouse(Position{0, 9}));

        field = board.field(Position{4, 4});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::West);
        REQUIRE(Board::isSource(Position{4, 4}));
        field = board.field(Position{5, 4});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::North);
        REQUIRE(Board::isSource(Position{5, 4}));
        field = board.field(Position{5, 5});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::East);
        REQUIRE(Board::isSource(Position{5, 5}));
        field = board.field(Position{4, 5});
        REQUIRE(field.stone() == Stone::OneCurveWithStop);
        REQUIRE(field.orientation() == Orientation::South);
        REQUIRE(Board::isSource(Position{4, 5}));

        field = board.field(Position{1, 1});
        REQUIRE(field.stone() == Stone::Empty);
        REQUIRE(field.orientation() == Orientation::North);
        REQUIRE(Board::isGarden(Position{1, 1}));
    }
};
