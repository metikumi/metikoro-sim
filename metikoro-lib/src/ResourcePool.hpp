// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <numeric>

#include "Setup.hpp"
#include "StonePool.hpp"


class ResourcePool {
    friend struct std::hash<ResourcePool>;

public:
    constexpr static uint8_t size = Stone::count - 1;
    using StoneCounts = std::array<uint8_t, size>;

public:
    ResourcePool() = default;

    public:
    [[nodiscard]] auto operator==(const ResourcePool &other) const -> bool { return _stoneCounts == other._stoneCounts; }
    [[nodiscard]] auto operator!=(const ResourcePool &other) const -> bool { return not (*this == other); }

public:
    [[nodiscard]] auto stoneCounts() const noexcept -> const StoneCounts& { return _stoneCounts; }
    [[nodiscard]] auto empty() const noexcept -> bool {
        return std::ranges::all_of(_stoneCounts, [](const uint8_t count) { return count == 0; });
    }

    [[nodiscard]] auto hasStone(const Stone stone) const noexcept -> bool { return count(stone) > 0; }

    [[nodiscard]] auto count(const Stone stone) const noexcept -> uint8_t {
        return at(stone);
    }

    void add(const Stone stone, const uint8_t count = 1) noexcept {
        at(stone) += count;
    }

    [[nodiscard]] auto withAdded(const Stone stone, const uint8_t count = 1) const noexcept -> ResourcePool {
        ResourcePool result = *this;
        result.add(stone, count);
        return result;
    }

    void take(const Stone stone, const uint8_t count = 1) {
        if (count > at(stone)) {
            throw Error("ResourcePool::take - Cannot take more stones than available in the pool.");
        }
        at(stone) -= count;
    }

    [[nodiscard]] auto withTaken(const Stone stone, const uint8_t count = 1) const noexcept -> ResourcePool {
        ResourcePool result = *this;
        result.take(stone, count);
        return result;
    }

    [[nodiscard]] auto allActionOneExtraDraw() const noexcept -> StoneList {
        return Stone::allNonEmpty()
            | std::views::filter([this](const Stone& stone) { return hasStone(stone); })
            | std::ranges::to<std::vector>();
    }

    [[nodiscard]] auto allActionTwoExtraDraws() const noexcept -> StonePairList {
        StonePairList result;
        for (const auto stoneA : Stone::allNonEmpty()) {
            for (const auto stoneB : Stone::allNonEmpty()) {
                if (stoneA == stoneB) {
                    if (count(stoneA) >= 2) {
                        result.emplace_back(stoneA, stoneB);
                    }
                } else {
                    if (stoneA < stoneB and hasStone(stoneA) and hasStone(stoneB)) {
                        result.emplace_back(stoneA, stoneB);
                    }
                }
            }
        }
        return result;
    }

    [[nodiscard]] auto allRegularDraws() const noexcept -> StoneList {
        if (empty()) {
            return {};
        }
        return Stone::allNonEmpty()
            | std::views::filter([this](const Stone& stone) { return hasStone(stone); })
            | std::ranges::to<std::vector>();
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return size * 2U;
    }

    void addToData(std::string &data) const noexcept {
        for (auto i = 0; i < size; ++i) {
            utility::addByteAsHexDigits(data, _stoneCounts[i]);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> ResourcePool {
        if (data.size() != dataSize()) {
            throw Error("ResourcePool: Invalid data size.");
        }
        ResourcePool result;
        for (std::size_t i = 0; i < result._stoneCounts.size(); ++i) {
            result._stoneCounts.at(i) = utility::hexStringToByte(data.substr(i * 2U, 2U));
        }
        return result;
    }

private:
    [[nodiscard]] auto at(const Stone stone) const -> uint8_t {
        const auto index = static_cast<std::size_t>(stone.type() - 1);
        if (index >= _stoneCounts.size()) {
            throw Error("ResourcePool::at - out of bounds");
        }
        return _stoneCounts.at(index);
    }
    [[nodiscard]] auto at(const Stone stone) -> uint8_t& {
        const auto index = static_cast<std::size_t>(stone.type() - 1);
        if (index >= _stoneCounts.size()) {
            throw Error("ResourcePool::at - out of bounds");
        }
        return _stoneCounts.at(index);
    }

private:
    StoneCounts _stoneCounts{};
};


static_assert(Serializable<ResourcePool>);


template<>
struct std::hash<ResourcePool> {
    auto operator()(const ResourcePool &resourcePool) const noexcept -> std::size_t {
        return utility::hashFromArray(resourcePool._stoneCounts);
    }
};
