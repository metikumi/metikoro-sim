// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "BoardArea.hpp"
#include "Field.hpp"
#include "Player.hpp"
#include "Utilities.hpp"


class FrameField : public Field {
public:
    FrameField() = default;

public:
    [[nodiscard]] auto operator==(const FrameField &other) const -> bool = default;

public:
    void setArea(const BoardArea area) noexcept { _area = area; }
    [[nodiscard]] auto area() const noexcept -> BoardArea { return _area; }
    void setPlayer(const Player player) noexcept { _player = player; }
    [[nodiscard]] auto player() const noexcept -> Player { return _player; }
    [[nodiscard]] auto isStatic() const noexcept -> bool {
        return _area == BoardArea::Frame || _area == BoardArea::House || _area == BoardArea::Source;
    }
    [[nodiscard]] auto toField() const noexcept -> Field { return Field{_data}; }

private:
    BoardArea _area{}; ///< The area of the board.
    Player _player{}; ///< For the house and garden, the player where this area belongs to.
};


template<>
struct std::hash<FrameField> {
    auto operator()(const FrameField &field) const noexcept -> std::size_t {
        return utility::hashFromValues(field, field.area());
    }
};

