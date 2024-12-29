// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#include "OrbMoves.hpp"


#include "OrbMoveGenerator.hpp"


auto OrbMoves::allForState(const GameState &state) noexcept -> OrbMoves {
    OrbMoveGenerator generator{state};
    return generator.allMoves();
}
