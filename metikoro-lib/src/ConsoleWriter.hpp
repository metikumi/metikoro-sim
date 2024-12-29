// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ConsoleColor.hpp"

#include <string>
#include <memory>


class ConsoleWriter;
using ConsoleWriterPtr = std::shared_ptr<ConsoleWriter>;


class ConsoleWriter {
public:
    using Color = ConsoleColor;

public:
    virtual ~ConsoleWriter() = default;

public:
    virtual void writeLog(const std::string_view &text, const Color color = Color::Default) noexcept {
        if (_consoleWriterForwarder) {
            _consoleWriterForwarder->writeLog(text, color);
        }
    }

    virtual void writeStatus(const std::string_view &text, const Color color) noexcept {
        if (_consoleWriterForwarder) {
            _consoleWriterForwarder->writeStatus(text, color);
        }
    }

    virtual void writeWaitingStatus(const std::string_view &text, const Color color) noexcept {
        if (_consoleWriterForwarder) {
            _consoleWriterForwarder->writeWaitingStatus(text, color);
        }
    }

public:
    void setConsoleWriterForwarder(const ConsoleWriterPtr &consoleWriterForwarder) noexcept {
        _consoleWriterForwarder = consoleWriterForwarder;
    }

private:
    ConsoleWriterPtr _consoleWriterForwarder;
};

