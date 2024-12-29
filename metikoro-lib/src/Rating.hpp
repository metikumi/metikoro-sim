// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Player.hpp"
#include "RatingPlayer.hpp"


/// Rating of a move or situation in general.
///
class Rating {
public:
    using RatingPerPlayer = std::array<RatingPlayer, Player::count>;

public:
    Rating() = default;
    Rating(double draws, const RatingPerPlayer &ratingsPerPlayer) noexcept
        : _draws(draws), _ratingsPerPlayer(ratingsPerPlayer) {
    }

public: // operators
    [[nodiscard]] constexpr auto operator==(const Rating &other) const noexcept -> bool = delete; // makes no sense
    [[nodiscard]] constexpr auto operator+(const Rating &other) const noexcept -> Rating {
        RatingPerPlayer resultRatings{};
        std::ranges::transform(
            _ratingsPerPlayer,
            other._ratingsPerPlayer,
            resultRatings.begin(),
            [](const RatingPlayer &a, const RatingPlayer &b) {
               return a + b; // Assuming RatingPlayer has operator+
            }
        );
        return {_draws + other._draws, resultRatings};
    }
    auto operator+=(const Rating &other) noexcept -> Rating& {
        *this = *this + other;
        return *this;
    }

public: // accessors
    /// The rating for draws happened for this situation.
    ///
    [[nodiscard]] auto draws() const noexcept -> double {
        return _draws;
    }

    /// The ratings for the individual players.
    ///
    [[nodiscard]] auto ratings() const noexcept -> const RatingPerPlayer& {
        return _ratingsPerPlayer;
    }

    /// The number of ratings. Usually the number of players.
    ///
    [[nodiscard]] auto ratingsSize() const noexcept -> std::size_t {
        return _ratingsPerPlayer.size();
    }

    /// Get one rating.
    ///
    [[nodiscard]] auto rating(const std::size_t index) const -> const RatingPlayer& {
        if (index >= _ratingsPerPlayer.size()) {
            throw Error("Rating::adjustRating: player out of bounds.");
        }
        return _ratingsPerPlayer.at(index);
    }

public: // modifiers
    void adjustDraws(const double delta) noexcept {
        _draws += delta;
    }
    void adjustRating(const Player player, const RatingPlayer &delta) {
        if (player.value() >= _ratingsPerPlayer.size()) {
            throw Error("Rating::adjustRating: player out of bounds.");
        }
        _ratingsPerPlayer.at(player) += delta;
    }
    void add(const Rating &other) noexcept {
        *this += other;
    }

public: // conversion
    [[nodiscard]] auto toString(double totalGames) const noexcept -> std::string {
        std::string perPlayerStr;
        auto player = 0;
        for (const auto &playerRating : _ratingsPerPlayer) {
            double combinedPercentage = playerRating.combined() / totalGames * 100.0;
            double winPercentage = playerRating.win() / totalGames * 100.0;
            double lossPercentage = playerRating.loss() / totalGames * 100.0;
            perPlayerStr += std::format(" P{}:{:4.2f}% W:{:3.2f}% L:{:3.2f}%", player, combinedPercentage, winPercentage, lossPercentage);
            player += 1;
        }
        double drawsPercentage = _draws / totalGames * 100.0;
        return std::format("D:{:6.2f}%{}", drawsPercentage, perPlayerStr);
    }

private:
    double _draws{0.0}; ///< The rating for draws
    RatingPerPlayer _ratingsPerPlayer{}; ///< The individual ratings for each player.
};

