// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "StringLines.hpp"

#include <cstdint>
#include <functional>


class GridOutput {
public:
    enum Style : uint8_t {
        BorderOnly = 0,
        Grid,
        GridBoldBorder,
        GridVertical,
    };

    enum Element : uint8_t {
        TopLeft = 0,
        TopLine,
        TopDivider,
        TopRight,
        CellLeft,
        CellMiddle,
        CellDivider,
        CellRight,
        DividerLeft,
        DividerMiddle,
        DividerCross,
        DividerRight,
        BottomLeft,
        BottomLine,
        BottomDivider,
        BottomRight,
    };

    using CellFn = std::function<std::string(const uint8_t, const uint8_t)>;

public:
    constexpr GridOutput(const Style style, const uint8_t cellWidth, const uint8_t columns, const uint8_t rows) noexcept
        : _style{style}, _cellWidth{cellWidth}, _columns{columns}, _rows{rows} {
    }

    [[nodiscard]] auto toLines(const CellFn& cellFn) const noexcept -> StringLines {
        StringLines lines;
        if (_style == BorderOnly) {
            lines.append(linePattern(TopLeft, TopLine, TopRight));
        } else {
            lines.append(linePattern(TopLeft, TopLine, TopDivider, TopRight));
        }
        for (uint8_t row = 0; row < _rows; ++row) {
            if (row > 0 && (_style != BorderOnly && _style != GridVertical)) {
                lines.append(linePattern(DividerLeft, DividerMiddle, DividerCross, DividerRight));
            }
            std::string line;
            line += element(CellLeft);
            for (uint8_t column = 0; column < _columns; ++column) {
                if (column > 0 && _style != BorderOnly) {
                    line += element(CellDivider);
                }
                auto cellContent = cellFn(column, row);
                line.append(cellContent);
            }
            line += element(CellRight);
            lines.append(line);
        }
        if (_style == BorderOnly) {
            lines.append(linePattern(BottomLeft, BottomLine, BottomRight));
        } else {
            lines.append(linePattern(BottomLeft, BottomLine, BottomDivider, BottomRight));
        }
        return lines;
    }

    [[nodiscard]] auto linePattern(Element left, Element middle, Element right) const noexcept -> std::string {
        std::string result;
        result += element(left);
        for (uint8_t i = 0; i < _columns * _cellWidth; ++i) {
            result += element(middle);
        }
        result += element(right);
        return result;;
    }

    [[nodiscard]] auto linePattern(Element left, Element middle, Element line, Element right) const noexcept -> std::string {
        std::string result;
        result += element(left);
        for (uint8_t i = 0; i < _columns; ++i) {
            if (i > 0) {
                result += element(line);
            }
            for (uint8_t j = 0; j < _cellWidth; ++j) {
                result += element(middle);
            }
        }
        result += element(right);
        return result;;
    }

    [[nodiscard]] auto element(const Element element) const noexcept -> std::string {
        static const std::array elements = {
            std::array<std::string, 16>{"┌","─","─","┐","│"," "," ","│","│"," "," ","│","└","─","─","┘"},
            std::array<std::string, 16>{"┌","─","┬","┐","│"," ","│","│","├","─","┼","┤","└","─","┴","┘"},
            std::array<std::string, 16>{"┏","━","┯","┓","┃"," ","│","┃","┠","─","┼","┨","┗","━","┷","┛"},
            std::array<std::string, 16>{"┌","─","┬","┐","│"," ","│","│","│"," ","│","│","└","─","┴","┘"},
        };
        return elements[_style][element];
    }

private:
    Style _style;
    uint8_t _cellWidth;
    uint8_t _columns;
    uint8_t _rows;
};
