// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ActionPool.hpp"


class ActionPools {
    friend struct std::hash<ActionPools>;

public:
    ActionPools() = default;

public: // operators
    [[nodiscard]] constexpr auto operator==(const ActionPools &other) const noexcept -> bool = default;
    [[nodiscard]] auto operator[](const Player player) const -> const ActionPool& {
        if (player.value() >= _actionPools.size()) {
            throw Error("ActionPools::[] Out of bounds");
        }
        return _actionPools.at(player);
    }
    [[nodiscard]] auto operator[](const Player player) -> ActionPool& {
        if (player.value() >= _actionPools.size()) {
            throw Error("ActionPools::[] Out of bounds");
        }
        return _actionPools.at(player);
    }

public:
    [[nodiscard]] auto active() const noexcept -> const ActionPool& {
        return _actionPools.front();
    }
    [[nodiscard]] auto active() noexcept -> ActionPool& {
        return _actionPools.front();
    }

    [[nodiscard]] auto rotated(const Rotation rotation) const noexcept -> ActionPools {
        return ActionPools(rotatedLeft(_actionPools, rotation.value()));
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return ActionPool::dataSize() * Player::count;
    }

    void addToData(std::string &data) const noexcept {
        for (const auto &pool : _actionPools) {
            pool.addToData(data);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> ActionPools {
        if (data.size() != dataSize()) {
            throw Error("ActionPools: Invalid data size.");
        }
        std::array<ActionPool, Player::count> actionPools;
        for (std::size_t i = 0; i < actionPools.size(); ++i) {
            actionPools.at(i) = ActionPool::fromData(data.substr(i * ActionPool::dataSize(), ActionPool::dataSize()));
        }
        return ActionPools{actionPools};
    }

private:
    template<typename T, std::size_t N>
    static auto rotatedLeft(const std::array<T, N> &arr, std::size_t rotations) -> std::array<T, N> {
        if (rotations == 0 || rotations % N == 0) {
            return arr; // No rotation needed
        }
        rotations %= N; // Normalize rotations to be within [0, N-1]
        std::array<T, N> temp;
        std::copy(arr.begin() + rotations, arr.end(), temp.begin());
        std::copy(arr.begin(), arr.begin() + rotations, temp.begin() + (N - rotations));
        return temp;
    }

    explicit ActionPools(std::array<ActionPool, Player::count> actionPools) : _actionPools(actionPools) {}

private:
    std::array<ActionPool, Player::count> _actionPools;
};


static_assert(Serializable<ActionPools>);


template<>
struct std::hash<ActionPools> {
    auto operator()(const ActionPools &actionPools) const noexcept -> std::size_t {
        return utility::hashFromArray(actionPools._actionPools);
    }
};

