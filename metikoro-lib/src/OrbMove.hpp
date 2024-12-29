// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Position.hpp"

#include <vector>

#include "Error.hpp"


class GameState;


/// An orb movement in the game
///
class OrbMove {
    friend struct std::hash<OrbMove>;

public:
    OrbMove() = default; // no move
    constexpr OrbMove(const Position start, const Position stop) noexcept : _start{start}, _stop{stop} {}

public: // operators
    auto operator==(const OrbMove &other) const noexcept -> bool = default;

public:
    [[nodiscard]] auto start() const noexcept -> Position { return _start; }
    [[nodiscard]] auto stop() const noexcept -> Position { return _stop; }
    [[nodiscard]] auto isNoMove() const noexcept -> bool { return _start == _stop; }
    [[nodiscard]] auto toString() const noexcept -> std::string {
        if (isNoMove()) {
            return "OrbMove(no move)";
        }
        return std::format("OrbMove({}->{})", _start.toString(), _stop.toString());
    }

public: // methods
    /// Applies this orb move to the given state.
    ///
    /// @throws Error in case this orb move is not valid for this state.
    ///
    void applyTo(GameState &state) const;

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return Position::dataSize() * 2U;
    }

    void addToData(std::string &data) const noexcept {
        if (isNoMove()) {
            data.append(dataSize(), '_');
        } else {
            _start.addToData(data);
            _stop.addToData(data);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> OrbMove {
        if (data.size() != dataSize()) {
            throw Error("OrbMove: Invalid data size.");
        }
        const auto start = Position::fromData(data.substr(0, Position::dataSize()));
        const auto stop = Position::fromData(data.substr(Position::dataSize(), Position::dataSize()));
        return OrbMove{start, stop};
    }

private:
    Position _start = Position::invalid(); ///< The current position of the orb / invalid = from resource pool.
    Position _stop = Position::invalid(); ///< The new position / invalid = no move.
};


static_assert(Serializable<OrbMove>);


template<>
struct std::hash<OrbMove> {
    auto operator()(const OrbMove &orbMove) const noexcept -> std::size_t {
        return utility::hashFromValues(orbMove._start, orbMove._stop);
    }
};

