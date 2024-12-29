// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "GameMove.hpp"
#include "GameState.hpp"


/// One game turn for one player.
///
struct GameTurn {
    std::size_t turn = 0; ///< The turn number. 0 = initial turn.
    Player activePlayer; ///< The active player of this turn.
    GameState state; ///< The state where this move is executed on, top-left corner = active player.
    GameMove gameMove; ///< The move this player did, or no move to mark the end of the game.
};

