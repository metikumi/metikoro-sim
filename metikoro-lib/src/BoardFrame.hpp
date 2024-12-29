// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "FrameField.hpp"
#include "FieldGrid.hpp"
#include "OrbPositions.hpp"


class BoardFrame : public FieldGrid<FrameField, setup::boardSize> {
public:
    static constexpr uint8_t houseOrbCount = 3;
    using HouseOrbPositions = std::array<Position, houseOrbCount>;
    using PlayerHouseOrbPositions = std::array<HouseOrbPositions, Player::count>;
    static constexpr uint8_t sourceOrbCount = 4;
    using SourceOrbPositions = std::array<Position, sourceOrbCount>;

public:
    BoardFrame() {
        Player player{0};
        for (const Rotation rotation : Rotation::allClockwise()) {
            auto set = [&](Position position, Stone stone, Orientation orientation, BoardArea area) {
                setFieldRotated(position, stone, orientation, rotation);
                setAreaRotated(position, area, rotation);
            };
            set(Position{0, 0}, Stone::OneCurveWithStop, Orientation::East,  BoardArea::House);
            set(Position{0, 1}, Stone::SwitchWithStop,   Orientation::North, BoardArea::House);
            set(Position{0, 2}, Stone::OneCurve,         Orientation::North, BoardArea::House);
            set(Position{0, 3}, Stone::Empty,            Orientation::North, BoardArea::Frame);
            set(Position{0, 4}, Stone::Empty,            Orientation::North, BoardArea::Frame);
            set(Position{1, 0}, Stone::SwitchWithStop,   Orientation::East,  BoardArea::House);
            set(Position{2, 0}, Stone::OneCurve,         Orientation::South, BoardArea::House);
            set(Position{3, 0}, Stone::Empty,            Orientation::North, BoardArea::Frame);
            set(Position{4, 0}, Stone::Empty,            Orientation::North, BoardArea::Frame);
            set(Position{1, 1}, Stone::Empty,            Orientation::North, BoardArea::Garden);
            set(Position{2, 1}, Stone::Empty,            Orientation::North, BoardArea::Garden);
            set(Position{3, 1}, Stone::Empty,            Orientation::North, BoardArea::Garden);
            set(Position{1, 2}, Stone::Empty,            Orientation::North, BoardArea::Garden);
            set(Position{2, 2}, Stone::Empty,            Orientation::North, BoardArea::Garden);
            set(Position{1, 3}, Stone::Empty,            Orientation::North, BoardArea::Garden);
            set(Position{setup::sourceOffset, setup::sourceOffset},
                Stone::OneCurveWithStop, Orientation::West, BoardArea::Source);
            // Set the source positions.
            _sourceOrbPositions[rotation] =
                rotatedPosition(rotation, Position{setup::sourceOffset, setup::sourceOffset});
            // Set the orb position to test.
            _houseOrbPositions[player] = {
                rotatedPosition(rotation, Position{0, 0}),
                rotatedPosition(rotation, Position{1, 0}),
                rotatedPosition(rotation, Position{0, 1})};
            // Assign player areas
            for (Length x = 0; x < setup::boardSize / 2; ++x) {
                for (Length y = 0; y < setup::boardSize / 2; ++y) {
                    const auto pos = rotatedPosition(rotation, Position{x, y});
                    field(pos).setPlayer(player);
                }
            }
            player.next();
        }
    }

public:
    void setAreaRotated(const Position position, const BoardArea area, const Rotation rotation) noexcept {
        field(rotatedPosition(rotation, position)).setArea(area);
    }

    [[nodiscard]] auto sourceOrbPositions() const noexcept -> const SourceOrbPositions& {
        return _sourceOrbPositions;
    }
    [[nodiscard]] auto houseOrbPositions(const Player player) const noexcept -> const HouseOrbPositions& {
        return _houseOrbPositions.at(player);
    }

private:
    PlayerHouseOrbPositions _houseOrbPositions{};
    SourceOrbPositions _sourceOrbPositions{};
};

