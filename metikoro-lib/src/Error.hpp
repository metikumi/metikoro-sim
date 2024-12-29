// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <exception>
#include <string>


class Error final : public std::exception {
public:
    explicit Error(std::string message) : _message{std::move(message)} {}

public:
    [[nodiscard]] auto what() const noexcept -> const char* override { return _message.c_str(); }

private:
    std::string _message;
};
