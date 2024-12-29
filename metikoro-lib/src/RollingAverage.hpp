// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <numeric>
#include <vector>


template <typename T, std::size_t MAX_N>
class RollingAverage {
    static_assert(MAX_N > 0);
    static_assert(std::is_floating_point_v<T>);

public:
    using Values = std::vector<T>;

public:
    RollingAverage() = default;

public:
    void add(const T value) noexcept {
        _values.push_back(value);
        if (_values.size() > MAX_N) {
            _values.erase(_values.begin());
        }
        _average = std::accumulate(_values.begin(), _values.end(), 0.0) / static_cast<T>(_values.size());
    }

    [[nodiscard]] auto average() const noexcept -> T {
        return _average;
    }

private:
    T _average = 0;
    Values _values{};
};

