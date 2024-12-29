// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "OrbPosition.hpp"
#include "Position.hpp"
#include "Rotation.hpp"
#include "Setup.hpp"


class OrbPositions {
    friend struct std::hash<OrbPositions>;

public:
    using Positions = std::array<OrbPosition, setup::orbCount>;

public:
    OrbPositions() = default;

public:
    [[nodiscard]] auto operator==(const OrbPositions &other) const -> bool = default;

public:
    [[nodiscard]] auto positions() const noexcept -> const Positions& { return _positions; }

    [[nodiscard]] auto koPosition(Position orbPosition) const noexcept -> Position {
        const auto it = std::ranges::find_if(
            _positions,
            [orbPosition](const OrbPosition &op) { return op.position == orbPosition; });
        if (it == _positions.end()) {
            return Position::invalid();
        }
        return it->koPosition;
    }

    [[nodiscard]] auto inGameCount() const noexcept -> uint8_t {
        auto it = std::ranges::find_if(_positions, isPosInvalid);
        if (it == _positions.end()) { return _positions.size(); }
        return static_cast<uint8_t>(std::distance(_positions.begin(), it));
    }

    [[nodiscard]] auto hasSpare() const noexcept -> bool {
        return inGameCount() < setup::orbCount;
    }

    [[nodiscard]] auto isOrbAt(Position position) const noexcept -> bool {
        return std::ranges::find_if(_positions, [position](const OrbPosition &op) {
            return op.position == position;
        }) != _positions.end();
    }

    [[nodiscard]] auto rotated(const Rotation rotation) const noexcept -> OrbPositions {
        auto result = *this;
        for (auto &op : result._positions) {
            op.position = op.position.rotated(rotation, setup::boardSize);
            op.koPosition = op.koPosition.rotated(rotation, setup::boardSize);
        }
        result.sort();
        return result;
    }

    void moveOrb(const Position oldPosition, const Position newPosition) {
        if (newPosition.isInvalid()) {
            throw Error("OrbPositions::moveOrb - new position is invalid.");
        }
        if (isOrbAt(newPosition)) {
            throw Error("OrbPositions::moveOrb - collision with existing orb.");
        }
        const auto it = std::ranges::find_if(
            _positions,
            [oldPosition](const OrbPosition &op) { return op.position == oldPosition; });
        if (it == _positions.end()) {
            throw Error("OrbPositions::moveOrb - no orb found at old position.");
        }
        if (it != _positions.end()) {
            it->koPosition = it->position;
            it->koLock = 3;
            it->position = newPosition;
        }
        sort();
    }

    void nextTurn() noexcept {
        for (auto &op : _positions) {
            if (op.koLock > 0) {
                op.koLock -= 1;
                if (op.koLock == 0) {
                    op.koPosition = Position::invalid();
                }
            }
        }
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return OrbPosition::dataSize() * setup::orbCount;
    }

    void addToData(std::string &data) const noexcept {
        for (const auto &orbPosition : _positions) {
            orbPosition.addToData(data);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> OrbPositions {
        if (data.size() != dataSize()) {
            throw Error("OrbPositions: Invalid data size.");
        }
        OrbPositions result;
        for (std::size_t i = 0; i < result._positions.size(); ++i) {
            result._positions.at(i) = OrbPosition::fromData(data.substr(i * OrbPosition::dataSize(), OrbPosition::dataSize()));
        }
        return result;
    }

private:
    void sort() noexcept {
        std::ranges::stable_sort(
            _positions, [](const OrbPosition &a, const OrbPosition &b) {
                return a.position < b.position;
            }
        );
    }

private:
    static auto isPosInvalid(const OrbPosition &op) noexcept -> bool {
        return op.position.isInvalid();
    }

private:
    Positions _positions;
};


static_assert(Serializable<OrbPositions>);


template<>
struct std::hash<OrbPositions> {
    auto operator()(const OrbPositions &orbPositions) const noexcept -> std::size_t {
        return utility::hashFromArray(orbPositions._positions);
    }
};

