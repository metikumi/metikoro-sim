// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


/// The rating for a single player.
///
class RatingPlayer {
public:
    RatingPlayer() = default;
    constexpr RatingPlayer(const double combined, const double win, const double loss) noexcept
        : _combined{combined}, _win{win}, _loss{loss} {
    }

public: // operators
    auto operator==(const RatingPlayer &other) const noexcept -> bool = delete; // makes no sense
    [[nodiscard]] constexpr auto operator+(const RatingPlayer &other) const noexcept -> RatingPlayer {
        return RatingPlayer{
            _combined + other._combined,
            _win + other._win,
            _loss + other._loss
        };
    }
    auto operator+=(const RatingPlayer &other) noexcept -> RatingPlayer& {
        *this = *this + other;
        return *this;
    }
    [[nodiscard]] constexpr auto operator/(const double divisor) const noexcept -> RatingPlayer {
        return RatingPlayer{
            _combined / divisor,
            _win / divisor,
            _loss / divisor
        };
    }


public: // accessors
    [[nodiscard]] auto combined() const noexcept -> double { return _combined; }
    [[nodiscard]] auto win() const noexcept -> double { return _win; }
    [[nodiscard]] auto loss() const noexcept -> double { return _loss; }

private:
    double _combined{0.0};
    double _win{0.0};
    double _loss{0.0};
};


