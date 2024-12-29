// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Utilities.hpp"

#include <algorithm>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>


class StringLines {
public:
    StringLines() = default;

public:
    [[nodiscard]] auto maximumLineLength() const noexcept -> std::size_t {
        return std::ranges::max(_lines | std::views::transform(utility::sizeUTF8), std::less<>{});
    }
    void append(const std::string &line) noexcept { _lines.push_back(line); }
    void append(std::string &&line) noexcept { _lines.emplace_back(std::move(line)); }
    void append(const StringLines &lines) noexcept {
        for (const auto &line : lines._lines) {
            _lines.emplace_back(line);
        }
    }
    void append(StringLines &&lines) noexcept {
        _lines.reserve(_lines.size() + lines._lines.size());
        for (auto &&line : lines._lines) {
            _lines.emplace_back(std::move(line));
        }
        lines._lines.clear();
    }
    /// Extend all lines to match the longest line.
    void extendLines() noexcept {
        auto length = maximumLineLength();
        for (auto& line : _lines) {
            utility::resizeUTF8(line, length);
        }
    }
    [[nodiscard]] auto toString() const noexcept -> std::string {
        std::string result;
        result.reserve(std::accumulate(_lines.begin(), _lines.end(), 0, [](int acc, const std::string &line) {
            return acc + line.size() + 1;
        }));
        for (std::size_t i = 0; i < _lines.size(); ++i) {
            if (i > 0) {
                result.append("\n");
            }
            result.append(_lines[i]);
        }
        return result;
    }

    /// Combines multiple blocks of equal sized lines to a column layout.
    ///
    static auto fromColumns(const std::vector<StringLines>& columns, uint8_t gapSpaces) -> StringLines {
        StringLines result;
        if (columns.empty()) {
            return result; // Return an empty result if no columns are provided.
        }
        // Find the maximum number of lines among the columns.
        size_t maxLines = 0;
        std::vector<std::size_t> maxLineLengths;
        for (const auto& column : columns) {
            maxLines = std::max(maxLines, column._lines.size());
            maxLineLengths.push_back(column.maximumLineLength());
        }
        // Precompute the gap string.
        std::string const gap(gapSpaces, ' ');
        // Construct the combined lines.
        for (size_t i = 0; i < maxLines; ++i) {
            std::string combinedLine;
            for (size_t col = 0; col < columns.size(); ++col) {
                if (col > 0) {
                    combinedLine += gap;
                }
                const auto &lines = columns[col]._lines;
                if (i < lines.size()) {
                    combinedLine += lines[i];
                } else {
                    combinedLine += std::string(maxLineLengths[col], ' '); // Pad to match column width.
                }
            }
            result.append(std::move(combinedLine));
        }
        return result;
    }

private:
    std::vector<std::string> _lines;
};

