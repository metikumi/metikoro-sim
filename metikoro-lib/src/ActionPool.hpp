// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Player.hpp"
#include "Setup.hpp"
#include "StonePool.hpp"


using ActionPool = StonePool<setup::actionPoolSize>;


static_assert(Serializable<ActionPool>);
