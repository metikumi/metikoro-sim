// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#include "Utilities.hpp"


#include <algorithm>
#include <ranges>


namespace utility {


auto sizeUTF8(const std::string_view &str) -> std::size_t {
    return std::ranges::count_if(str, [](unsigned char c) {
        return (c & 0xC0) != 0x80; // A UTF-8 continuation byte has the two highest bits as `10`.
    });
}


void resizeUTF8(std::string &str, std::size_t newSize, char fill) {
    auto currentLength = sizeUTF8(str);
    if (newSize < currentLength) {
        // Truncate the string to the desired number of UTF-8 characters
        std::size_t charCount = 0;
        auto it = str.begin();
        while (it != str.end() && charCount < newSize) {
            if ((*it & 0xC0) != 0x80) {
                ++charCount;
            }
            ++it;
        }
        str.erase(it, str.end());
    } else if (newSize > currentLength) {
        // Append the fill character until the string reaches the desired size
        while (currentLength < newSize) {
            str.append(1, fill); // Append one fill character
            ++currentLength;
        }
    }
}


}

