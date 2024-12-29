// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "OrbMove.hpp"

#include <ranges>
#include <__algorithm/ranges_any_of.h>


class GameState;


/// A list of orb movements.
///
class OrbMoves {
public:
    using Moves = std::vector<OrbMove>;

public:
    OrbMoves() = default;
    explicit OrbMoves(const std::vector<OrbMove> &moves) noexcept : _moves{moves} {}

public:
    [[nodiscard]] auto operator==(const OrbMoves &other) const noexcept -> bool = default;
    [[nodiscard]] auto operator[](const std::size_t index) const noexcept -> const OrbMove& { return _moves.at(index); }

public: // accessors
    [[nodiscard]] auto moves() const noexcept -> const Moves& { return _moves; }
    [[nodiscard]] auto size() const noexcept -> size_t { return _moves.size(); }
    [[nodiscard]] auto empty() const noexcept -> bool { return _moves.empty(); }
    [[nodiscard]] auto begin() const noexcept -> Moves::const_iterator { return _moves.begin(); }
    [[nodiscard]] auto end() const noexcept -> Moves::const_iterator { return _moves.end(); }
    [[nodiscard]] auto at(const size_t index) const noexcept -> const OrbMove& { return _moves.at(index); }
    [[nodiscard]] auto contains(const OrbMove orbMove) const noexcept -> bool {
        return std::ranges::find(_moves, orbMove) != _moves.end();
    }

public: // modifiers
    void add(const OrbMove &move) noexcept { _moves.emplace_back(move); }
    void add(OrbMove &&move) noexcept { _moves.emplace_back(move); }
    void add(const OrbMoves &moves) noexcept { _moves.insert(_moves.end(), moves.begin(), moves.end()); }
    void clear() noexcept { _moves.clear(); }

public: // methods
    /// Generate all possible orb movements for the current player in the given state.
    ///
    [[nodiscard]] static auto allForState(const GameState &state) noexcept -> OrbMoves;

private:
    Moves _moves;
};