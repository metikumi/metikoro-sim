// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ActionSequences.hpp"


#include "ActionGenerator.hpp"


auto ActionSequences::allForState(const GameState &state) noexcept -> ActionSequences {
    return ActionGenerator{state}.all();
}


