// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Orientations.hpp"


class OrientationsTest final : public el::UnitTest {
public:
    Orientations ori;

    void testDefault() {
        ori = {};
        REQUIRE(ori.empty());
    }

    void testSingle() {
        ori = Orientation::North;
        REQUIRE(ori.contains(Orientation::North));
        REQUIRE_FALSE(ori.contains(Orientation::East));
        REQUIRE_FALSE(ori.contains(Orientation::South));
        REQUIRE_FALSE(ori.contains(Orientation::West));
        ori = Orientation::East;
        REQUIRE_FALSE(ori.contains(Orientation::North));
        REQUIRE(ori.contains(Orientation::East));
        REQUIRE_FALSE(ori.contains(Orientation::South));
        REQUIRE_FALSE(ori.contains(Orientation::West));
        ori = Orientation::South;
        REQUIRE_FALSE(ori.contains(Orientation::North));
        REQUIRE_FALSE(ori.contains(Orientation::East));
        REQUIRE(ori.contains(Orientation::South));
        REQUIRE_FALSE(ori.contains(Orientation::West));
        ori = Orientation::West;
        REQUIRE_FALSE(ori.contains(Orientation::North));
        REQUIRE_FALSE(ori.contains(Orientation::East));
        REQUIRE_FALSE(ori.contains(Orientation::South));
        REQUIRE(ori.contains(Orientation::West));
    }

    void testCombinations() {
        ori = {Orientation::North, Orientation::East, Orientation::South, Orientation::West};
        REQUIRE(ori.contains(Orientation::North));
        REQUIRE(ori.contains(Orientation::East));
        REQUIRE(ori.contains(Orientation::South));
        REQUIRE(ori.contains(Orientation::West));
    }
};

