// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <cassert>
#include <cstddef>
#include <functional>


#define ALL_COMPARE_OPERATORS(Class, Variable) \
    constexpr auto operator==(const Class &other) const noexcept -> bool { return Variable == other.Variable; } \
    constexpr auto operator!=(const Class &other) const noexcept -> bool { return Variable != other.Variable; } \
    constexpr auto operator<(const Class &other) const noexcept -> bool { return Variable < other.Variable; } \
    constexpr auto operator<=(const Class &other) const noexcept -> bool { return Variable <= other.Variable; } \
    constexpr auto operator>(const Class &other) const noexcept -> bool { return Variable > other.Variable; } \
    constexpr auto operator>=(const Class &other) const noexcept -> bool { return Variable >= other.Variable; } \
    constexpr auto operator<=>(const Class &other) const noexcept { return Variable <=> other.Variable; }

#define ALL_COMPARE_WITH_NATIVE_OPERATORS(Native, Variable) \
    constexpr auto operator==(Native value) const noexcept -> bool { return Variable == value; } \
    constexpr auto operator!=(Native value) const noexcept -> bool { return Variable != value; } \
    constexpr auto operator<(Native value) const noexcept -> bool { return Variable < value; } \
    constexpr auto operator<=(Native value) const noexcept -> bool { return Variable <= value; } \
    constexpr auto operator>(Native value) const noexcept -> bool { return Variable > value; } \
    constexpr auto operator>=(Native value) const noexcept -> bool { return Variable >= value; } \
    constexpr auto operator<=>(Native value) const noexcept { return Variable <=> value; }


namespace utility {


constexpr auto isHexDigit(const char digit) noexcept -> bool {
    return (digit >= '0' && digit <= '9') || (digit >= 'a' && digit <= 'f') || (digit >= 'A' && digit <= 'F');
}


constexpr auto valueToHexDigit(const uint8_t value) noexcept -> char {
    static constexpr auto digits = std::array<char, 16>{
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
    return digits.at(value);
}

constexpr void addByteAsHexDigits(std::string &data, const uint8_t value) noexcept {
    data += valueToHexDigit(value >> 4);
    data += valueToHexDigit(value & 0x0fU);
}

/// Convert a hexadecimal digit into a value.
///
/// Any non-hexadecimal digit is converted into zero. This is used e.g. to convert `_` into 0!
///
/// @return The value of the hex digit.
///
constexpr auto hexDigitToValue(const char digit) noexcept -> uint8_t {
    if (digit >= '0' && digit <= '9') { return digit - '0'; }
    if (digit >= 'a' && digit <= 'f') { return digit - 'a' + 10; }
    if (digit >= 'A' && digit <= 'F') { return digit - 'A' + 10; }
    return 0;
}

constexpr auto hexStringToByte(const std::string_view &data) noexcept -> uint8_t {
    return hexDigitToValue(data[0]) << 4 | hexDigitToValue(data[1]);
}

template <typename T>
void combineHash(std::size_t &seed, const T &value) {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


template <typename... Args>
auto hashFromValues(Args... args) {
    std::size_t seed{};
    (combineHash(seed, args), ...);
    return seed;
}


template <typename T, std::size_t N>
auto hashFromArray(const std::array<T, N> &arr) -> std::size_t {
    std::size_t seed{};
    for (const auto &value : arr) {
        combineHash(seed, value);
    }
    return seed;
}

/// Get the number of characters in the given UTF-8 string.
///
/// @param str The string.
/// @return The number of characters.
///
[[nodiscard]] auto sizeUTF8(const std::string_view &str) -> std::size_t;

/// Resize the given UTF-8 string to fill the given number of *characters*.
///
/// @param str The string to resize.
/// @param newSize The new size in characters.
/// @param fill The character to fill.
///
void resizeUTF8(std::string &str, std::size_t newSize, char fill = ' ');


}

