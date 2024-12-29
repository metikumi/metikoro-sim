// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "RatingAdjustment.hpp"

#include <cstdint>


class RatingGame : public Rating {
public:
    RatingGame() = default;

public: // accessors
    [[nodiscard]] auto ratingCount() const noexcept -> uint64_t { return _ratingCount; }
    [[nodiscard]] auto drawsNormal() const noexcept -> double {
        return draws() / static_cast<double>(_ratingCount);
    }
    [[nodiscard]] auto ratingNormal(const std::size_t index) const noexcept -> RatingPlayer {
        return rating(index) / static_cast<double>(_ratingCount);
    }

public: // modifiers
    void applyAdjustment(const RatingAdjustment &adjustment) noexcept {
        _ratingCount += 1;
        add(adjustment);
    }

public: // conversion
    using Rating::toString;
    [[nodiscard]] auto toString() const noexcept -> std::string {
        return std::format("C:{: >5} {}", _ratingCount, Rating::toString(_ratingCount));
    }

private:
    uint64_t _ratingCount{0}; ///< The number of ratings.
};

