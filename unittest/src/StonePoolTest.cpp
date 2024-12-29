// Copyright (c) 2024-2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "StonePool.hpp"


class StonePoolTest : public el::UnitTest {
public:
    StonePool<6> pool;

    void testEmpty() {
        pool = {};
        REQUIRE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 6);
        REQUIRE(pool.stones() == std::array<Stone, 6>{Stone::Empty, Stone::Empty, Stone::Empty, Stone::Empty, Stone::Empty, Stone::Empty});
        REQUIRE(pool.at(0) == Stone::Empty);
        REQUIRE(pool.at(1) == Stone::Empty);
        REQUIRE(pool.at(2) == Stone::Empty);
        REQUIRE(pool.at(3) == Stone::Empty);
        REQUIRE(pool.at(4) == Stone::Empty);
        REQUIRE(pool.at(5) == Stone::Empty);
        REQUIRE_FALSE(pool.hasStone(Stone::Crossing));
        REQUIRE_FALSE(pool.hasStone(Stone::TwoCurves));
        REQUIRE_FALSE(pool.hasStones(Stone::Crossing, Stone::TwoCurves));
        REQUIRE(pool.stoneCount() == 0);
        REQUIRE(pool.uniqueStones().empty());
        REQUIRE(pool.uniqueStonePairs().empty());
        REQUIRE(pool.uniqueStoneQuads().empty());

        StonePool<6> const pool2;
        REQUIRE(pool == pool2);
    }

    void testAdd() {
        pool = {};
        pool.add(Stone::Crossing);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 5);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::Crossing,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty});
        REQUIRE(pool.at(0) == Stone::Crossing);
        REQUIRE(pool.at(1) == Stone::Empty);
        REQUIRE(pool.at(2) == Stone::Empty);
        REQUIRE(pool.at(3) == Stone::Empty);
        REQUIRE(pool.at(4) == Stone::Empty);
        REQUIRE(pool.at(5) == Stone::Empty);
        REQUIRE(pool.hasStone(Stone::Crossing));
        REQUIRE_FALSE(pool.hasStone(Stone::TwoCurves));
        REQUIRE_FALSE(pool.hasStones(Stone::Crossing, Stone::TwoCurves));
        REQUIRE(pool.stoneCount() == 1);
        REQUIRE(pool.uniqueStones() == std::vector<Stone>{Stone::Crossing});

        pool.add(Stone::TwoCurves);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 4);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::TwoCurves,
            Stone::Crossing,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty});
        REQUIRE(pool.at(0) == Stone::TwoCurves);
        REQUIRE(pool.at(1) == Stone::Crossing);
        REQUIRE(pool.at(2) == Stone::Empty);
        REQUIRE(pool.at(3) == Stone::Empty);
        REQUIRE(pool.at(4) == Stone::Empty);
        REQUIRE(pool.at(5) == Stone::Empty);
        REQUIRE(pool.hasStone(Stone::Crossing));
        REQUIRE(pool.hasStone(Stone::TwoCurves));
        REQUIRE(pool.hasStones(Stone::Crossing, Stone::TwoCurves));
        REQUIRE_FALSE(pool.hasStones(Stone::Crossing, Stone::Crossing));
        REQUIRE_FALSE(pool.hasStones(Stone::TwoCurves, Stone::TwoCurves));
        REQUIRE(pool.stoneCount() == 2);
        REQUIRE(pool.uniqueStones() == std::vector<Stone>{Stone::TwoCurves, Stone::Crossing});

        pool.add(Stone::Crossing);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 3);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::TwoCurves,
            Stone::Crossing,
            Stone::Crossing,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty});
        REQUIRE(pool.at(0) == Stone::TwoCurves);
        REQUIRE(pool.at(1) == Stone::Crossing);
        REQUIRE(pool.at(2) == Stone::Crossing);
        REQUIRE(pool.at(3) == Stone::Empty);
        REQUIRE(pool.at(4) == Stone::Empty);
        REQUIRE(pool.at(5) == Stone::Empty);
        REQUIRE(pool.hasStone(Stone::Crossing));
        REQUIRE(pool.hasStone(Stone::TwoCurves));
        REQUIRE(pool.hasStones(Stone::Crossing, Stone::TwoCurves));
        REQUIRE(pool.hasStones(Stone::TwoCurves, Stone::Crossing));
        REQUIRE(pool.hasStones(Stone::Crossing, Stone::Crossing));
        REQUIRE(pool.stoneCount() == 3);
        REQUIRE(pool.uniqueStones() == std::vector<Stone>{Stone::TwoCurves, Stone::Crossing});

        pool.add(Stone::SwitchC);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 2);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::Crossing,
            Stone::Crossing,
            Stone::Empty,
            Stone::Empty});
        REQUIRE(pool.at(0) == Stone::SwitchC);
        REQUIRE(pool.at(1) == Stone::TwoCurves);
        REQUIRE(pool.at(2) == Stone::Crossing);
        REQUIRE(pool.at(3) == Stone::Crossing);
        REQUIRE(pool.at(4) == Stone::Empty);
        REQUIRE(pool.at(5) == Stone::Empty);
        REQUIRE(pool.hasStone(Stone::Crossing));
        REQUIRE(pool.hasStone(Stone::TwoCurves));
        REQUIRE(pool.hasStone(Stone::SwitchC));
        REQUIRE(pool.hasStones(Stone::Crossing, Stone::TwoCurves));
        REQUIRE(pool.hasStones(Stone::TwoCurves, Stone::Crossing));
        REQUIRE(pool.hasStones(Stone::SwitchC, Stone::Crossing));
        REQUIRE(pool.hasStones(Stone::Crossing, Stone::SwitchC));
        REQUIRE(pool.stoneCount() == 4);
        REQUIRE(pool.uniqueStones() == std::vector<Stone>{Stone::SwitchC, Stone::TwoCurves, Stone::Crossing});

        pool.add(Stone::CrossingWithStop);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 1);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::CrossingWithStop,
            Stone::Crossing,
            Stone::Crossing,
            Stone::Empty});
        REQUIRE(pool.at(0) == Stone::SwitchC);
        REQUIRE(pool.at(1) == Stone::TwoCurves);
        REQUIRE(pool.at(2) == Stone::CrossingWithStop);
        REQUIRE(pool.at(3) == Stone::Crossing);
        REQUIRE(pool.at(4) == Stone::Crossing);
        REQUIRE(pool.at(5) == Stone::Empty);
        REQUIRE(pool.hasStone(Stone::Crossing));
        REQUIRE(pool.hasStone(Stone::TwoCurves));
        REQUIRE(pool.hasStone(Stone::SwitchC));
        REQUIRE(pool.hasStone(Stone::CrossingWithStop));
        REQUIRE(pool.hasStones(Stone::Crossing, Stone::TwoCurves));
        REQUIRE(pool.hasStones(Stone::SwitchC, Stone::CrossingWithStop));
        REQUIRE(pool.stoneCount() == 5);
        REQUIRE(pool.uniqueStones() == std::vector<Stone>{
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::CrossingWithStop,
            Stone::Crossing});

        pool.add(Stone::CrossingWithStop);
        REQUIRE_FALSE(pool.empty());
        REQUIRE(pool.full());
        REQUIRE(pool.freeSlots() == 0);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::CrossingWithStop,
            Stone::CrossingWithStop,
            Stone::Crossing,
            Stone::Crossing});
        REQUIRE(pool.at(0) == Stone::SwitchC);
        REQUIRE(pool.at(1) == Stone::TwoCurves);
        REQUIRE(pool.at(2) == Stone::CrossingWithStop);
        REQUIRE(pool.at(3) == Stone::CrossingWithStop);
        REQUIRE(pool.at(4) == Stone::Crossing);
        REQUIRE(pool.at(5) == Stone::Crossing);
        REQUIRE(pool.hasStone(Stone::Crossing));
        REQUIRE(pool.hasStone(Stone::TwoCurves));
        REQUIRE(pool.hasStone(Stone::SwitchC));
        REQUIRE(pool.hasStone(Stone::CrossingWithStop));
        REQUIRE(pool.hasStones(Stone::Crossing, Stone::TwoCurves));
        REQUIRE(pool.hasStones(Stone::SwitchC, Stone::CrossingWithStop));
        REQUIRE(pool.hasStones(Stone::CrossingWithStop, Stone::CrossingWithStop));
        REQUIRE(pool.stoneCount() == 6);
        REQUIRE(pool.uniqueStones() == std::vector<Stone>{
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::CrossingWithStop,
            Stone::Crossing});
    }

    void testFull() {
        pool = {};
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        REQUIRE(pool.full());
    }

    void testRemove() {
        pool = {};
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::TwoCurves);
        pool.add(Stone::CrossingWithStop);
        pool.add(Stone::SwitchC);
        pool.add(Stone::SwitchC);
        REQUIRE_FALSE(pool.empty());
        REQUIRE(pool.full());
        REQUIRE(pool.freeSlots() == 0);
        REQUIRE(pool.stoneCount() == 6);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::CrossingWithStop,
            Stone::Crossing,
            Stone::Crossing});
        pool.take(Stone::CrossingWithStop);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 1);
        REQUIRE(pool.stoneCount() == 5);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::Crossing,
            Stone::Crossing,
            Stone::Empty});
        pool.take(Stone::Crossing);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 2);
        REQUIRE(pool.stoneCount() == 4);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::Crossing,
            Stone::Empty,
            Stone::Empty});
        pool.take(Stone::SwitchC);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 3);
        REQUIRE(pool.stoneCount() == 3);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::TwoCurves,
            Stone::Crossing,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty});
        pool.take(Stone::TwoCurves);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 4);
        REQUIRE(pool.stoneCount() == 2);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::Crossing,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty});
        pool.take(Stone::Crossing);
        REQUIRE_FALSE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 5);
        REQUIRE(pool.stoneCount() == 1);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::SwitchC,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty});
        pool.take(Stone::SwitchC);
        REQUIRE(pool.empty());
        REQUIRE_FALSE(pool.full());
        REQUIRE(pool.freeSlots() == 6);
        REQUIRE(pool.stoneCount() == 0);
        REQUIRE(pool.stones() == std::array<Stone, 6>{
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty,
            Stone::Empty});
    }

    void testUniqueStonePairs() {
        pool = {};
        REQUIRE(pool.uniqueStonePairs().empty());
        REQUIRE(pool.uniqueStoneQuads().empty());
        pool.add(Stone::Crossing);
        pool.add(Stone::CrossingWithStop);
        pool.add(Stone::TwoCurves);
        pool.add(Stone::SwitchA);
        pool.add(Stone::SwitchB);
        pool.add(Stone::SwitchC);
        REQUIRE(pool.uniqueStonePairs().size() == 30);
        REQUIRE(pool.uniqueStoneQuads().size() == 360);

        pool = {};
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        REQUIRE(pool.uniqueStonePairs().size() == 1);
        REQUIRE(pool.uniqueStoneQuads().empty());
        REQUIRE(pool.uniqueStonePairs() == std::vector<StonePair>{StonePair{Stone::Crossing, Stone::Crossing}});

        pool = {};
        pool.add(Stone::Crossing);
        pool.add(Stone::TwoCurves);
        REQUIRE(pool.uniqueStonePairs().size() == 2);
        REQUIRE(pool.uniqueStoneQuads().empty());
        REQUIRE(pool.uniqueStonePairs() == std::vector<StonePair>{
            StonePair{Stone::TwoCurves, Stone::Crossing},
            StonePair{Stone::Crossing, Stone::TwoCurves}
        });

        pool = {};
        pool.add(Stone::Crossing);
        pool.add(Stone::TwoCurves);
        pool.add(Stone::SwitchA);
        REQUIRE(pool.uniqueStonePairs().size() == 6);
        REQUIRE(pool.uniqueStoneQuads().empty());
        REQUIRE(pool.uniqueStonePairs() == std::vector<StonePair>{
            StonePair{Stone::SwitchA, Stone::TwoCurves},
            StonePair{Stone::SwitchA, Stone::Crossing},
            StonePair{Stone::TwoCurves, Stone::SwitchA},
            StonePair{Stone::TwoCurves, Stone::Crossing},
            StonePair{Stone::Crossing, Stone::SwitchA},
            StonePair{Stone::Crossing, Stone::TwoCurves},
        });
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        REQUIRE(pool.uniqueStonePairs().size() == 7);
        REQUIRE(pool.uniqueStonePairs() == std::vector<StonePair>{
            StonePair{Stone::SwitchA, Stone::TwoCurves},
            StonePair{Stone::SwitchA, Stone::Crossing},
            StonePair{Stone::TwoCurves, Stone::SwitchA},
            StonePair{Stone::TwoCurves, Stone::Crossing},
            StonePair{Stone::Crossing, Stone::SwitchA},
            StonePair{Stone::Crossing, Stone::TwoCurves},
            StonePair{Stone::Crossing, Stone::Crossing},
        });

        pool = {};
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        pool.add(Stone::Crossing);
        REQUIRE(pool.uniqueStoneQuads().size() == 1);
        REQUIRE(pool.uniqueStoneQuads() == std::vector<StoneQuad>{
            StoneQuad{Stone::Crossing, Stone::Crossing, Stone::Crossing, Stone::Crossing}});
        pool.add(Stone::TwoCurves);
        REQUIRE(pool.uniqueStoneQuads().size() == 5);
        REQUIRE(pool.uniqueStoneQuads() == std::vector<StoneQuad>{
            StoneQuad{Stone::TwoCurves, Stone::Crossing, Stone::Crossing, Stone::Crossing},
            StoneQuad{Stone::Crossing, Stone::TwoCurves, Stone::Crossing, Stone::Crossing},
            StoneQuad{Stone::Crossing, Stone::Crossing, Stone::TwoCurves, Stone::Crossing},
            StoneQuad{Stone::Crossing, Stone::Crossing, Stone::Crossing, Stone::TwoCurves},
            StoneQuad{Stone::Crossing, Stone::Crossing, Stone::Crossing, Stone::Crossing}});
    }
};