// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <concepts>
#include <string>
#include <string_view>


template <typename T>
concept Serializable = requires(T obj, std::string& str, std::string_view data) {
    { T::dataSize() } -> std::same_as<std::size_t>;
    { obj.addToData(str) } noexcept;
    { T::fromData(data) } -> std::same_as<T>;
};

