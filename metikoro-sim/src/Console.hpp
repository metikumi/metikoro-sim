// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ConsoleColor.hpp"
#include "ConsoleWriter.hpp"

#include <iostream>
#include <memory>
#include <mutex>


class Console;
using ConsolePtr = std::shared_ptr<Console>;


class Console final : public ConsoleWriter {
    using ConsoleWriter::writeLog;

    enum class Display {
        Normal,
        OneLineStatus,
        SimulationStatus
    };

public:
    using ColorMap = std::map<Color, std::tuple<int, int>>;

public:
    Console() = default;

public: // Implement ConsoleWriter
    void writeLog(const std::string_view &text, const Color textColor) noexcept override {
        std::unique_lock const lock{_mutex};
        clearStatus(Display::Normal);
        write(text, textColor);
        writeLineBreak();
    }

    void writeStatus(const std::string_view &text, const Color textColor) noexcept override {
        std::unique_lock const lock{_mutex};
        if (_colorEnabled) {
            clearStatus(Display::OneLineStatus);
            writeBeforeStatus();
            writeHeader(text, Color::DarkGray, textColor);
            writeAfterStatus();
        } else {
            write(text, textColor);
            writeLineBreak();
        }
    }

    void writeWaitingStatus(const std::string_view &text, const Color textColor) noexcept override {
        std::unique_lock const lock{_mutex};
        if (_colorEnabled) {
            clearStatus(Display::OneLineStatus);
            writeBeforeStatus();
            writeHeaderWithAnimatedSymbol(text, Color::DarkGray, Color::White, textColor);
            writeAfterStatus();
        } else {
            write(text, textColor);
            writeLineBreak();
        }
    }

public:
    void setColorEnabled(bool enabled) noexcept {
        _colorEnabled = enabled;
    }

    [[nodiscard]] auto colorEnabled() const noexcept -> bool {
        return _colorEnabled;
    }

    void setConsoleWidth(const std::size_t width) noexcept {
        _consoleWidth = width;
    }

    [[nodiscard]] auto consoleWidth() const noexcept -> std::size_t {
        return _consoleWidth;
    }

    void writeSimulationStatus(
        const RatingGame &rating,
        const double gamesPerHour,
        const double moveAverage,
        const std::string_view &backendStatus) noexcept {

        std::unique_lock const lock{_mutex};

        clearStatus(Display::SimulationStatus);
        writeBeforeStatus();
        writeHeaderWithAnimatedSymbol("Simulation Running", Color::White, Color::Green, Color::LightGreen);
        constexpr auto labelWidth = 16;
        writeGameCountField("Games", labelWidth, rating.ratingCount(), gamesPerHour, Color::White, Color::BrightWhite);
        writeDoubleField("Move Avg.", labelWidth, moveAverage, Color::White, Color::BrightWhite);
        writePercentageField("Draws", labelWidth, rating.drawsNormal(), Color::White, Color::BrightWhite, Color::Orange);
        writeFieldLabel("Backend:", Color::White, labelWidth);
        auto backendColor = Color::White;
        if (backendStatus.contains("ERROR")) {
            backendColor = Color::Red;
        } else if (backendStatus.contains("OK")) {
            backendColor = Color::Green;
        }
        write(backendStatus, backendColor);
        writeFillToEnd(" ", Color::Default);
        writeLineBreak();
        for (std::size_t i = 0; i < rating.ratingsSize(); ++i) {
            writeHeader(std::format("Player {}:", i), Color::White, Color::LightBlue);
            auto ratingNormal = rating.ratingNormal(i);
            writePlusMinusField("Combined", labelWidth, ratingNormal.combined(), Color::White, Color::BrightWhite, Color::Green, Color::Red);
            writePercentageField("Wins", labelWidth, ratingNormal.win(), Color::White, Color::BrightWhite, Color::Green);
            writePercentageField("Losses", labelWidth, ratingNormal.loss(), Color::White, Color::BrightWhite, Color::Red);
        }
        writeAfterStatus();
    }

private:
    void writeBeforeStatus() noexcept {
        writeEscape("[?25l"); // hide cursor
    }

    void writeAfterStatus() noexcept {
        writeEscape(std::format("[{}A", _currentLine)); // move the cursor back to the first status line.
        writeEscape("[?25h"); // make the cursor visible again.
        flush();
    }

    void writeGameCountField(
        const std::string_view &labelCount,
        const std::size_t labelWidth,
        const int64_t count,
        const double countPerHour,
        const Color labelColor,
        const Color valueColor) noexcept {

        writeFieldLabel(labelCount, labelColor, labelWidth);
        write(std::format("{:> 10}", count), valueColor);
        write(" => ", Color::DarkGray);
        write(std::format("{:> 16.1f} / h ", countPerHour), valueColor);
        writeFillToEnd(" ", Color::Default);
        writeLineBreak();
    }

    void writeDoubleField(
        const std::string_view &labelCount,
        const std::size_t labelWidth,
        const double value,
        const Color labelColor,
        const Color valueColor) noexcept {

        writeFieldLabel(labelCount, labelColor, labelWidth);
        write(std::format("{:> 10.2f}", value), valueColor);
        writeFillToEnd(" ", Color::Default);
        writeLineBreak();
    }

    void writePlusMinusField(
        const std::string_view &label,
        const std::size_t labelWidth,
        const double normal,
        const Color labelColor,
        const Color valueColor,
        const Color plusBarColor,
        const Color minusBarColor) noexcept {

        writeFieldLabel(label, labelColor, labelWidth);
        write(std::format("{:> 9.2f}% ", normal * 100.0), valueColor);
        const auto clampedNormal = std::clamp(normal, -1.0, 1.0);
        const auto firstNormal = clampedNormal >= 0.0 ? 1.0 : 1.0 + clampedNormal;
        const auto secondNormal = clampedNormal >= 0.0 ? clampedNormal : 0.0;
        const auto availableColumns = remainingColumns() - 3U;
        const auto firstColumns = availableColumns / 2;
        const auto secondColumns = availableColumns - firstColumns;
        write("[", Color::White);
        writeBar(firstNormal, "░", Color::DarkGray, "█", minusBarColor, firstColumns);
        write("|", Color::White);
        writeBar(secondNormal, "█", plusBarColor, "░", Color::DarkGray, secondColumns);
        write("]", Color::White);
        writeLineBreak();
    }

    void writePercentageField(
        const std::string_view &label,
        const std::size_t labelWidth,
        const double normal,
        const Color labelColor,
        const Color valueColor,
        const Color barColor) noexcept {

        writeFieldLabel(label, labelColor, labelWidth);
        write(std::format("{:> 9.2f}% ", normal * 100.0), valueColor);
        auto availableColumns = remainingColumns() - 2U;
        write("[", Color::White);
        writeBar(std::clamp(normal, 0.0, 1.0), "█", barColor, "░", Color::DarkGray, availableColumns);
        write("]", Color::White);
        writeLineBreak();
    }

    void writeBar(
        const double normal,
        const std::string_view &leftFill,
        const Color leftColor,
        const std::string_view &rightFill,
        const Color rightColor,
        const std::size_t columns) noexcept {

        const auto leftColumns = static_cast<std::size_t>(std::round(static_cast<double>(columns) * normal));
        const auto rightColumns = columns - leftColumns;
        writeFill(leftFill, leftColumns, leftColor);
        writeFill(rightFill, rightColumns, rightColor);
    }

    void writeFieldLabel(const std::string_view &label, const Color textColor, std::size_t width) noexcept {
        write("  ", textColor);
        write(label, textColor);
        writeFillToColumn(".", textColor, width - 2);
        write(": ", textColor);
    }

    void writeHeaderWithAnimatedSymbol(
        const std::string_view &title,
        const Color lineColor,
        const Color symbolColor,
        const Color titleColor) {

        static std::array<std::string_view, 9> const symbols = {
            "▖", "▘", "▝", "▗", "▄", "▙", "█", "▟", "▄"
        };
        writeHeaderWithSymbol(symbols.at(_waitSymbolIndex), title, lineColor, symbolColor, titleColor);
        _waitSymbolIndex = (_waitSymbolIndex + 1) % symbols.size();
    }

    void writeHeaderWithSymbol(
        const std::string_view &symbol,
        const std::string_view &title,
        const Color lineColor,
        const Color symbolColor,
        const Color titleColor) {

        write("──[", lineColor);
        write(symbol, symbolColor);
        write("]─[", lineColor);

        if (title.size() > remainingColumns() - 2U) {
            write(title.substr(0, remainingColumns() - 2U), titleColor);
        } else {
            write(title, titleColor);
        }
        write("]", lineColor);
        writeFillToEnd("─", lineColor);
        writeLineBreak();
    }

    void writeHeader(const std::string_view &title, const Color lineColor, const Color titleColor) {
        write("──[", lineColor);
        if (title.size() > remainingColumns() - 2U) {
            write(title.substr(0, remainingColumns() - 2U), titleColor);
        } else {
            write(title, titleColor);
        }
        write("]", lineColor);
        writeFillToEnd("─", lineColor);
        writeLineBreak();
    }

    void writeFillToEnd(const std::string_view &fillChar, const Color fillColor) noexcept {
        writeFillToColumn(fillChar, fillColor, _consoleWidth);
    }

    void writeFillToColumn(const std::string_view &fillChar, const Color fillColor, const std::size_t column) noexcept {
        if (_currentColumn >= column) {
            return;
        }
        writeFill(fillChar, column - _currentColumn, fillColor);
    }

    void writeFill(const std::string_view &fillChar, const std::size_t count, const Color fillColor) noexcept {
        for (std::size_t i = 0; i < count; ++i) {
            write(fillChar, fillColor);
        }
    }

    static void flush() noexcept {
        std::cout.flush();
    }

    void clearStatus(const Display forDisplay) noexcept {
        if (_colorEnabled) {
            if (forDisplay != _lastDisplay) {
                writeEscape("[0J"); // delete from this position to the end of the screen.
                writeEscape("[0m"); // reset color.
                _lastDisplay = forDisplay;
            }
        }
        _currentColor = Color::Default;
        _currentColumn = 0;
        _currentLine = 0;
    }

    void write(const std::string_view &text, const Color textColor) noexcept {
        writeColor(textColor);
        std::cout << text;
        _currentColumn += utility::sizeUTF8(text);
    }

    void writeEscape(const std::string_view &data) noexcept {
        std::cout << "\x1b" << data;
    }

    void writeColor(const Color color) noexcept {
        if (_colorEnabled && color != _currentColor) {
            if (color == Color::Default) {
                writeEscape("[0m");
            } else {
                writeEscape(std::format("[{}m", std::get<0>(colorMap().at(color))));
            }
            _currentColor = color;
        }
    }

    void writeLineBreak() noexcept {
        std::cout << "\n";
        _currentColumn = 0;
        _currentLine += 1;
    }

    static auto colorMap() noexcept -> const ColorMap& {
        static ColorMap const result = {
            {Color::Black,       std::make_tuple(30, 40)},
            {Color::DarkRed,     std::make_tuple(31, 41)},
            {Color::Green,       std::make_tuple(32, 42)},
            {Color::Orange,      std::make_tuple(33, 43)},
            {Color::DarkBlue,    std::make_tuple(34, 44)},
            {Color::Violet,      std::make_tuple(35, 45)},
            {Color::DarkCyan,    std::make_tuple(36, 46)},
            {Color::White,       std::make_tuple(37, 47)},
            {Color::DarkGray,    std::make_tuple(90, 90)},
            {Color::Red,         std::make_tuple(91, 101)},
            {Color::LightGreen,  std::make_tuple(92, 102)},
            {Color::Yellow,      std::make_tuple(93, 103)},
            {Color::LightBlue,   std::make_tuple(94, 104)},
            {Color::Magenta,     std::make_tuple(95, 105)},
            {Color::Cyan,        std::make_tuple(96, 106)},
            {Color::BrightWhite, std::make_tuple(97, 107)},
        };
        return result;
    }

    [[nodiscard]] auto remainingColumns() const noexcept -> std::size_t {
        return _consoleWidth - _currentColumn;
    }

private:
    mutable std::mutex _mutex;
    bool _colorEnabled{true};
    std::size_t _consoleWidth{79};

    std::size_t _waitSymbolIndex{0};
    Display _lastDisplay{Display::Normal};
    std::size_t _currentColumn{0};
    std::size_t _currentLine{0};
    Color _currentColor{Color::Default};
};

