// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "Utilities.hpp"


class GameResult {
public:
    enum Value : uint8_t {
        None,
        Win,
        Draw,
        Loss,
    };

public:
    GameResult() = default;
    constexpr GameResult(const Value value) noexcept : _value{value} {}

public: // operators
    ALL_COMPARE_OPERATORS(GameResult, _value);
    ALL_COMPARE_WITH_NATIVE_OPERATORS(Value, _value);
    [[nodiscard]] explicit operator uint8_t() const { return _value; }

public: // attributes
    [[nodiscard]] auto value() const noexcept -> Value { return _value; }
    [[nodiscard]] auto isWin() const noexcept -> bool { return _value == Win; }
    [[nodiscard]] auto isDraw() const noexcept -> bool { return _value == Draw; }
    [[nodiscard]] auto isLoss() const noexcept -> bool { return _value == Loss; }

public: // methods
    [[nodiscard]] auto toString() const noexcept -> std::string {
        switch (_value) {
        case Win: return "Win";
        case Draw: return "Draw";
        case Loss: return "Loss";
        default: return "None";
        }
    }

private:
    Value _value{None};
};

