// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#include "Action.hpp"


#include "Error.hpp"
#include "GameState.hpp"


void Action::applyTo(GameState &state) const {
    switch (type()) {
    case PlaceStone: applyPlaceAction(state); break;
    case ReplaceStone: applyReplaceAction(state); break;
    case RotateStone: applyRotateAction(state); break;
    case DrawStone: applyDrawAction(state); break;
    default: break;
    }
}


void Action::applyPlaceAction(GameState &state) const {
    if (not state.actionPools().active().hasStone(actionStone())) {
        throw Error("Tried to place a stone that is not in the action pool.");
    }
    if (not state.board().canPlayerPlaceStone(position())) {
        throw Error("Tried to place a stone on a position where a stone is already placed.");
    }
    state.board().setField(position(), actionStone(), orientation());
    state.actionPools().active().take(actionStone());
}


void Action::applyReplaceAction(GameState &state) const {
    auto &pool = state.actionPools().active();
    if (pool.stoneCount() < 2) {
        throw Error("Replace action requires at least two stones in the action pool.");
    }
    if (not pool.hasStones(actionStone(), droppedStone())) {
        throw Error("The action or dropped stone are missing in the action pool.");
    }
    if (not state.board().canPlayerReplaceStone(position(), actionStone(), orientation())) {
        throw Error("Tried to replace a stone on an invalid position or where it does not change the situation.");
    }
    state.resourcePool().add(state.board().field(position()).stone());
    pool.take(actionStone());
    state.board().setField(position(), actionStone(), orientation());
    pool.take(droppedStone());
    state.resourcePool().add(droppedStone());
}


void Action::applyRotateAction(GameState &state) const {
    auto &pool = state.actionPools().active();
    if (pool.empty()) {
        throw Error("Rotate action requires at least one stone in the action pool.");
    }
    if (not pool.hasStone(droppedStone())) {
        throw Error("Tried to replace a stone that is not in the action pool.");
    }
    if (not actionStone().empty()) {
        throw Error("Rotation action contains an action stone.");
    }
    if (not state.board().canPlayerRotateStone(position(), orientation())) {
        throw Error("Cannot rotate stone on this position or rotation.");
    }
    state.board().setNewOrientation(position(), orientation());
    pool.take(droppedStone());
    state.resourcePool().add(droppedStone());
}


void Action::applyDrawAction(GameState &state) const {
    auto &pool = state.actionPools().active();
    if (pool.full()) {
        throw Error("Draw action requires at least one free slot in the action pool.");
    }
    if (not state.resourcePool().hasStone(actionStone())) {
        throw Error("Tried to draw a stone that is not in the resource pool.");
    }
    state.resourcePool().take(actionStone());
    pool.add(actionStone());
}
