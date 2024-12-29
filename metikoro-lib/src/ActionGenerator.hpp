// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ActionSequences.hpp"
#include "GameState.hpp"


class ActionGenerator {
    using AddActionSeqFn = std::function<void(const ActionSequence&)>;

public:
    explicit ActionGenerator(const GameState &state) : _state(state) {}

public:
    [[nodiscard]] auto all() const noexcept -> ActionSequences {
        ActionSequences actions;
        actions.reserve(countAllActions());
        addAllActions([&actions](const ActionSequence &actionSeq) {
            actions.add(actionSeq);
        });
        return actions;
    }

    [[nodiscard]] auto countAllActions() const noexcept -> std::size_t {
        std::size_t count = 0;
        addAllActions([&count](const ActionSequence&) { ++count; });
        return count;
    }

    void addAllActions(const AddActionSeqFn &addFn) const noexcept {
        const auto stoneCount = actionPools().active().stoneCount();
        addActionsPlace(addFn, stoneCount);
        addActionsReplace(addFn, stoneCount);
        addActionsRotate(addFn, stoneCount);
        addActionsExtraDraw(addFn);
    }

    static void forAllForPlace(
        const PositionList &positions,
        const StoneList &stones,
        const std::function<void(Position, Stone, Orientation)> &fn) noexcept {

        for (const auto position : positions) {
            for (const auto stone : stones) {
                auto uniqueOrientations = stone.uniqueOrientations();
                for (const auto orientation : Orientation::all()) {
                    if (uniqueOrientations.contains(orientation)) {
                        fn(position, stone, orientation);
                    }
                }
            }
        }
    }

    static void forAllForPlace(
        const PositionPairList &positionPairs,
        const StonePairList &stonePairs,
        const std::function<void(PositionPair, StonePair, OrientationPair)> &fn) noexcept {

        for (const auto &positionPair : positionPairs) {
            for (const auto &stonePair : stonePairs) {
                const auto uniqueOrientationsA = stonePair.first.uniqueOrientations();
                const auto uniqueOrientationsB = stonePair.second.uniqueOrientations();
                for (const auto orientationA : Orientation::all()) {
                    for (const auto orientationB : Orientation::all()) {
                        if (uniqueOrientationsA.contains(orientationA) && uniqueOrientationsB.contains(orientationB)) {
                            fn(positionPair, stonePair, {orientationA, orientationB});
                        }
                    }
                }
            }
        }
    }

    void forAllForRotation(
        const PositionList &positions,
        const StoneList &droppedStones,
        const std::function<void(Position, Orientation, Stone)> &fn) const noexcept {

        for (const auto position : positions) {
            if (orbPositions().isOrbAt(position)) {
                continue;
            }
            const auto &field = board().field(position);
            if (not field.canRotate()) {
                continue;
            }
            const auto currentOrientation = field.orientation();
            const auto uniqueOrientations = field.uniqueOrientations();
            for (const auto orientation : Orientation::all()) {
                if (orientation != currentOrientation and uniqueOrientations.contains(orientation)) {
                    for (const auto droppedStone : droppedStones) {
                        fn(position, orientation, droppedStone);
                    }
                }
            }
        }
    }

    void forAllForRotation(
        const PositionPairList &positionPairs,
        const StonePairList &droppedStonePairs,
        const std::function<void(PositionPair, OrientationPair, StonePair)> &fn) const noexcept {

        for (const auto &positionPair : positionPairs) {
            if (orbPositions().isOrbAt(positionPair.first) || orbPositions().isOrbAt(positionPair.second)) {
                continue;
            }
            const auto &fieldA = board().field(positionPair.first);
            const auto &fieldB = board().field(positionPair.second);
            if (not fieldA.canRotate() or not fieldB.canRotate()) {
                continue;
            }
            const auto currentOrientationA = fieldA.orientation();
            const auto currentOrientationB = fieldB.orientation();
            const auto uniqueOrientationsA = fieldA.uniqueOrientations();
            const auto uniqueOrientationsB = fieldB.uniqueOrientations();
            for (const auto orientationA : Orientation::all()) {
                for (const auto orientationB : Orientation::all()) {
                    if (orientationA != currentOrientationA and uniqueOrientationsA.contains(orientationA) and
                        orientationB != currentOrientationB and uniqueOrientationsB.contains(orientationB)) {
                        for (const auto &droppedStonePair : droppedStonePairs) {
                            fn(positionPair, {orientationA, orientationB}, droppedStonePair);
                        }
                    }
                }
            }
        }
    }

    void forAllForReplace(
        const PositionList &positions,
        const StonePairList &stonePairs,
        const std::function<void(Position, StonePair, Orientation)> &fn) const noexcept {

        for (const auto position : positions) {
            if (orbPositions().isOrbAt(position)) {
                continue;
            }
            for (const auto &stonePair : stonePairs) {
                const auto orientations = stonePair.first.uniqueOrientations();
                for (const auto orientation : Orientation::all()) {
                    if (orientations.contains(orientation)) {
                        fn(position, stonePair, orientation);
                    }
                }
            }
        }
    }

    void forAllForReplace(
        const PositionPairList &positionPairs,
        const StoneQuadList &stoneQuads,
        const std::function<void(PositionPair, StonePair, StonePair, OrientationPair)> &fn) const noexcept {

        for (const auto &positionPair : positionPairs) {
            if (orbPositions().isOrbAt(positionPair.first) || orbPositions().isOrbAt(positionPair.second)) {
                continue;
            }
            for (const auto &stoneQuad : stoneQuads) {
                const auto orientationsA = std::get<0>(stoneQuad).uniqueOrientations();
                const auto orientationsB = std::get<1>(stoneQuad).uniqueOrientations();
                for (const auto orientationA : Orientation::all()) {
                    for (const auto orientationB : Orientation::all()) {
                        if (orientationsA.contains(orientationA) and orientationsB.contains(orientationB)) {
                            fn(
                                positionPair,
                                {std::get<0>(stoneQuad), std::get<1>(stoneQuad)},
                                {std::get<2>(stoneQuad), std::get<3>(stoneQuad)},
                                {orientationA, orientationB});
                        }
                    }
                }
            }
        }
    }

    void addActionsPlace(const AddActionSeqFn &addFn, const uint8_t stoneCount) const noexcept {
        if (stoneCount < 1) {
            return;
        }
        forAllForPlace(board().allPlaceOneActionPositions(), actionPools().active().uniqueStones(),
            [&](const Position position, const Stone stone, const Orientation orientation) {
                addFn(Action::createPlace(position, stone, orientation));
            }
        );
        if (stoneCount < 2) {
            return;
        }
        forAllForPlace(board().allPlaceTwoActionPositions(), actionPools().active().uniqueStonePairs(),
            [&](PositionPair pos, StonePair stone, OrientationPair orientationPair) {
                addFn(std::array{
                    Action::createPlace(pos.first, stone.first, orientationPair.first),
                    Action::createPlace(pos.second, stone.second, orientationPair.second)
                });
            }
        );
    }

    void addActionsReplace(const AddActionSeqFn &addFn, const uint8_t stoneCount) const noexcept {
        if (stoneCount < 2) {
            return;
        }
        forAllForReplace(board().allReplaceOneActionPositions(), actionPools().active().uniqueStonePairs(),
            [&](const Position position, const StonePair stone, const Orientation orientation) {
                if (board().canPlayerReplaceStone(position, stone.first, orientation)) {
                    addFn(Action::createReplace(position, stone.first, orientation, stone.second));
                }
            }
        );
        if (stoneCount < 4) {
            return;
        }
        forAllForReplace(board().allReplaceTwoActionPositions(), actionPools().active().uniqueStoneQuads(),
            [&](PositionPair pos, StonePair as, StonePair ds, OrientationPair orientationPair) {
                if (board().canPlayerReplaceStone(pos.first, as.first, orientationPair.first) && // action possible?
                    board().canPlayerReplaceStone(pos.second, as.second, orientationPair.second)) {
                    addFn(std::array{
                        Action::createReplace(pos.first, as.first, orientationPair.first, ds.first),
                        Action::createReplace(pos.second, as.second, orientationPair.second, ds.second)
                    });
                }
            }
        );
    }

    void addActionsRotate(const AddActionSeqFn &addFn, const uint8_t stoneCount) const noexcept {
        if (stoneCount < 1) {
            return;
        }
        forAllForRotation(board().allRotateOneActionPositions(), actionPools().active().uniqueStones(),
            [&](const Position position, const Orientation orientation, const Stone droppedStone) {
                if (board().canPlayerRotateStone(position, orientation)) {
                    addFn(Action::createRotate(position, orientation, droppedStone));
                }
            }
        );
        if (stoneCount >= 2) {
            forAllForRotation(board().allRotateTwoActionPositions(), actionPools().active().uniqueStonePairs(),
                [&](PositionPair pos, OrientationPair orientationPair, StonePair ds) {
                    if (board().canPlayerRotateStone(pos.first, orientationPair.first) &&
                        board().canPlayerRotateStone(pos.second, orientationPair.second)) {

                        addFn(std::array{
                            Action::createRotate(pos.first, orientationPair.first, ds.first),
                            Action::createRotate(pos.second, orientationPair.second, ds.second)});
                    }
                }
            );
        }
    }

    void addActionsExtraDraw(const AddActionSeqFn &addFn) const noexcept {
        const auto freeSlots = actionPools().active().freeSlots();
        if (freeSlots > 1) { // need at least 2 free slots for extra draw
            for (const auto stone : resourcePool().allActionOneExtraDraw()) {
                addFn(ActionSequence({Action::createDraw(stone)}));
            }
        }
        if (freeSlots > 2) { // need at least 3 free slots for two extra draws
            for (const auto [stoneA, stoneB] : resourcePool().allActionTwoExtraDraws()) {
                addFn(ActionSequence({Action::createDraw(stoneA), Action::createDraw(stoneB)}));
            }
        }
    }

private:
    [[nodiscard]] auto actionPools() const noexcept -> const ActionPools& { return _state.actionPools(); }
    [[nodiscard]] auto board() const noexcept -> const Board& { return _state.board(); }
    [[nodiscard]] auto orbPositions() const noexcept -> const OrbPositions& { return _state.orbPositions(); }
    [[nodiscard]] auto resourcePool() const noexcept -> const ResourcePool& { return _state.resourcePool(); }

private:
    GameState _state;
};

