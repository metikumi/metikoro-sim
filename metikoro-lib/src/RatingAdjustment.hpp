// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "GameTurn.hpp"
#include "Player.hpp"
#include "Rating.hpp"


/// The adjustment of a rating, that can be applied to the rating of a game.
///
class RatingAdjustment : public Rating {
public:
    static constexpr auto ratingBase = 1.0;
    static constexpr auto deltaForWin = ratingBase;
    static constexpr auto deltaForLoss = ratingBase / static_cast<double>(Player::count - 1);
    static constexpr auto combinedDeltaForWin = ratingBase;
    static constexpr auto combinedDeltaForDraw = ratingBase / static_cast<double>(Player::count) * 0.1;
    static constexpr auto combinedDeltaForLoss = -ratingBase / static_cast<double>(Player::count - 1);

public:
    /// Zero adjustment.
    ///
    RatingAdjustment() = default;

    /// Create new score adjustments for the given turn.
    ///
    /// @param turn The turn.
    /// @param totalTurnCount The total turn count for the game.
    /// @param winningPlayer The winning player, or no player if a draw.
    ///
    constexpr RatingAdjustment(
        const GameTurn &turn,
        const std::size_t totalTurnCount,
        const std::optional<Player> winningPlayer) noexcept {

        const auto factor = adjustmentFactor(turn.turn, totalTurnCount);
        auto actualPlayer = turn.activePlayer;
        for (uint8_t i = 0; i < Player::count; ++i, actualPlayer.next()) {
            if (not winningPlayer.has_value()) {
                adjustDraws(ratingBase);
                adjustRating(Player{i}, RatingPlayer{
                    combinedDeltaForDraw * factor, 0.0, 0.0
                });
            } else if (actualPlayer == winningPlayer.value()) {
                adjustRating(Player{i}, RatingPlayer{
                    combinedDeltaForWin * factor, deltaForWin, 0.0
                });
            } else {
                adjustRating(Player{i}, RatingPlayer{
                    combinedDeltaForLoss * factor, 0.0, deltaForLoss
                });
            }
        }
    }

    /// Create new manual score adjustments with factor 1.0.
    ///
    /// @param winningPlayer The winning player, or no player if a draw.
    ///
    constexpr RatingAdjustment(const std::optional<Player> winningPlayer) noexcept {
        for (uint8_t i = 0; i < Player::count; ++i) {
            if (not winningPlayer.has_value()) {
                adjustDraws(ratingBase);
                adjustRating(Player{i}, RatingPlayer{combinedDeltaForDraw, 0.0, 0.0});
            } else if (i == winningPlayer.value()) {
                adjustRating(Player{i}, RatingPlayer{combinedDeltaForWin, deltaForWin, 0.0});
            } else {
                adjustRating(Player{i}, RatingPlayer{combinedDeltaForLoss, 0.0, deltaForLoss});
            }
        }
    }

private:
    /// Calculate the adjustment factor.
    ///
    [[nodiscard]] constexpr static auto adjustmentFactor(std::size_t turn, std::size_t totalTurnCount) noexcept -> double {
        auto resultFactor = static_cast<double>(turn - totalTurnCount) / static_cast<double>(turn);
        resultFactor = std::max(0.0001, resultFactor);
        resultFactor = std::min(1.0, resultFactor);
        return resultFactor;
    }
};


using RatingAdjustments = std::vector<RatingAdjustment>;

