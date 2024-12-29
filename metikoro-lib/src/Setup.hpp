// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Position.hpp"
#include "Stone.hpp"

#include <cstddef>


namespace setup {


/// The total side-length of the board, including the frame.
///
constexpr static Length boardSize = 10U;

/// The offset from the top left corner to the source field.
///
constexpr static Length sourceOffset = 4U;

/// The number of slots in the action pool.
///
constexpr static std::size_t actionPoolSize = 6;

/// The total number of orbs that are in the game (or spare).
///
constexpr static std::size_t orbCount = 9;

/// The number of orbs required in the house to win the game.
///
constexpr static uint8_t orbCountToWin = 3;

/// The number of loops to mark a game as a draw.
///
constexpr static std::size_t loopCountForDraw = 10;


constexpr static std::array<std::pair<uint8_t, Stone>, 7> resourcePoolStones = {
    std::make_pair(20U, Stone::Crossing),
    {20U, Stone::TwoCurves},
    {8U, Stone::CrossingWithStop},
    {8U, Stone::SwitchA},
    {8U, Stone::SwitchB},
    {8U, Stone::SwitchC},
    {8U, Stone::CurveWithBounces}
};

constexpr static std::array<std::pair<uint8_t, Stone>, 3> actionPoolStones = {
    std::make_pair(3, Stone::Crossing),
    {2, Stone::TwoCurves},
    {1, Stone::CrossingWithStop},
};


}

