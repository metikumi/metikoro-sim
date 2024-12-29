// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Stone.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <ranges>
#include <tuple>
#include <vector>

#include "Board.hpp"
#include "Field.hpp"


template<uint8_t N>
class StonePool {
public:
    static constexpr uint8_t capacity = N;
    using Stones = std::array<Stone, N>;

public:
    StonePool() = default;

public:
    [[nodiscard]] auto operator==(const StonePool &other) const -> bool { return _stones == other._stones; }
    [[nodiscard]] auto operator!=(const StonePool &other) const -> bool { return not (*this == other); }

public:
    [[nodiscard]] auto stones() const noexcept -> const Stones& { return _stones; }
    [[nodiscard]] auto at(uint8_t index) const noexcept -> Stone {
        return _stones.at(index);
    }
    [[nodiscard]] auto hasStone(Stone stone) const -> bool {
        if (stone == Stone::Empty) {
            throw Error("Tried to check for Empty stone.");
        }
        return std::ranges::find(_stones, stone) != _stones.end();
    }
    [[nodiscard]] auto hasStones(Stone stoneA, Stone stoneB) const -> bool {
        if (stoneA == Stone::Empty or stoneB == Stone::Empty) {
            throw Error("Tried to check for Empty stone.");
        }
        if (stoneA == stoneB) {
            return std::ranges::count(_stones, stoneA) >= 2;
        }
        return (std::ranges::find(_stones, stoneA) != _stones.end()) &&
            (std::ranges::find(_stones, stoneB) != _stones.end());
    }
    [[nodiscard]] auto empty() const noexcept -> bool {
        return _stones.front().type() == Stone::Type::Empty;
    }
    [[nodiscard]] auto full() const noexcept -> bool {
        return _stones.back().type() != Stone::Type::Empty;
    }
    [[nodiscard]] auto stoneCount() const noexcept -> uint8_t {
        const auto it = std::ranges::find_if(_stones,
            [](const Stone& stone) { return stone == Stone::Empty; });
        if (it == _stones.end()) { return N; }
        return static_cast<uint8_t>(std::distance(_stones.begin(), it));
    }
    [[nodiscard]] auto freeSlots() const noexcept -> uint8_t {
        return N - stoneCount();
    }

    void add(const Stone stone) {
        if (full()) {
            throw Error("Stone pool is full");
        }
        if (stone == Stone::Empty) {
            throw Error("Tried to add an empty stone to the pool.");
        }
        if (empty()) {
            _stones.front() = stone;
            return;
        }
        const auto it = std::ranges::find_if(
            _stones, [stone](const Stone &s) { return s < stone; });
        if (it == _stones.end()) {
            throw Error("Unexpected error: could not find an insert position");
        }
        std::copy_backward(it, _stones.end() - 1, _stones.end());
        *it = stone;
    }

    void take(Stone stone) {
        const auto it = std::ranges::find(_stones, stone);
        if (it == _stones.end()) {
            throw Error("Stone not found in pool");
        }
        if (it != (_stones.end() - 1)) {
            std::copy(it + 1, _stones.end(), it);
        }
        _stones.back() = Stone::Empty;
    }

    [[nodiscard]] auto uniqueStones() const noexcept -> StoneList {
        if (empty()) {
            return {};
        }
        StoneList result;
        result.reserve(N);
        for (const auto stone : _stones) {
            if (stone == Stone::Empty) {
                break;
            }
            if (std::ranges::find(result, stone) == result.end()) {
                result.push_back(stone);
            }
        }
        return result;
    }

    [[nodiscard]] auto uniqueStonePairs() const noexcept -> std::vector<std::pair<Stone, Stone>> {
        if (stoneCount() < 2) {
            return {};
        }
        std::vector<std::pair<Stone, Stone>> result;
        for (auto it = _stones.begin(); it != _stones.end() && *it != Stone::Empty; ++it) {
            for (auto jt = _stones.begin(); jt != _stones.end() && *jt != Stone::Empty; ++jt) {
                if (it == jt) { continue; }
                if (std::ranges::find(result, std::make_pair(*it, *jt)) == result.end()) {
                    result.emplace_back(*it, *jt);
                }
            }
        }
        return result;
    }

    [[nodiscard]] auto uniqueStoneQuads() const noexcept -> StoneQuadList {
        const auto stoneCount = this->stoneCount();
        if (stoneCount < 4) {
            return {};
        }
        StoneQuadList result;
        for (auto a = 0; a < stoneCount; ++a) {
            for (auto b = 0; b < stoneCount; ++b) {
                if (a == b) { continue; }
                for (auto c = 0; c < stoneCount; ++c) {
                    if (a == c || b == c) { continue; }
                    for (auto d = 0; d < stoneCount; ++d) {
                        if (a == d || b == d || c == d) { continue; }
                        auto value = std::make_tuple(
                            _stones.at(a),
                            _stones.at(b),
                            _stones.at(c),
                            _stones.at(d));
                        if (std::ranges::find(result, value) == result.end()) {
                            result.emplace_back(value);
                        }
                    }
                }
            }
        }
        return result;
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return N * Stone::dataSize();
    }

    void addToData(std::string &data) const noexcept {
        for (Stone stone : _stones) {
            data.append(stone.toString());
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> StonePool {
        if (data.size() != dataSize()) {
            throw Error("StonePool: Invalid data size.");
        }
        StonePool result;
        for (auto i = 0; i < N; ++i) {
            result._stones.at(i) = Stone::fromData(data.substr(i * Stone::dataSize(), Stone::dataSize()));
        }
        return result;
    }

protected:
    Stones _stones;
};


template<uint8_t N>
struct std::hash<StonePool<N>> {
    auto operator()(const StonePool<N> &stonePool) const noexcept -> std::size_t {
        return utility::hashFromArray(stonePool.stones());
    }
};

