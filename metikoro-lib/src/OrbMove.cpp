// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#include "OrbMove.hpp"


#include "GameState.hpp"


void OrbMove::applyTo(GameState &state) const {
    if (isNoMove()) {
        throw Error("Tried to apply no move.");
    }
    const auto oldIsSource = Board::isSource(_start);
    const auto newIsSource = Board::isSource(_stop);
    if (Board::isHouse(_start) and not Board::isHouse(_stop)) {
        throw Error("Tried to remove an orb from the house.");
    }
    if (not oldIsSource and newIsSource) {
        throw Error("Tried to move an orb back to the source.");
    }
    if (not state.board().field(_stop).hasStop()) {
        throw Error("Tried to move the orb to a field with no stop.");
    }
    if (not state.orbPositions().isOrbAt(_start)) {
        throw Error("Tried to move an orb at a location where is no orb.");
    }
    if (state.orbPositions().isOrbAt(_stop)) {
        throw Error("Tried to move the orb to a position where an orb is already placed.");
    }
    if (state.orbPositions().koPosition(_start) == _stop) {
        throw Error("Tried to move the orb back to its previous position (ko lock).");
    }
    state.orbPositions().moveOrb(_start, _stop);
    // If we move an orb from the source into the field, replace it at the source with a spare.
    if (oldIsSource and not newIsSource and state.orbPositions().hasSpare()) {
        state.orbPositions().moveOrb(Position::invalid(), _start);
    }
}

