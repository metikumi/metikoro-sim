// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <algorithm>

#include "AgentRegistry.hpp"
#include "BackendRegistry.hpp"
#include "BackendMemory.hpp"
#include "Console.hpp"
#include "SQLiteBackend.hpp"


class Configuration final : public ConsoleWriter {
    constexpr static auto introLine = "MetiKoro Simulation - Version 1.0";

public:
    enum ParseResult : uint8_t {
        StartSimulation,
        DisplayedHelp,
        DisplayedVersion,
    };

public:
    explicit Configuration(ConsolePtr console) : _console{std::move(console)} {
        setConsoleWriterForwarder(_console);
        _backendRegistry.add<BackendMemory>("memory");
        _backendRegistry.add<SQLiteBackend>("sqlite");
        _agentRegistry.add<AgentRandom>("random");
    }

public:
    void displayIntro() {
        writeLog(introLine);
        writeLog(std::format("> Simulation with {} threads.", _threads));
        if (_maximumGames > 0) {
            writeLog(std::format("> Maximum number of games: {}", _maximumGames));
        } else {
            writeLog("> Unlimited number of games. Press Ctrl+C to stop the simulation.");
        }
        writeLog(std::format("> Using backend: {}", _backendName));
        if (_backend != nullptr) {
            _backend->displayConfiguration();
        }
        for (std::size_t i = 0; i < _agents.size(); ++i) {
            writeLog(std::format("> Player Agent {}: {} {}", (i + 1), _agentNames[i], _agents[i]->configurationString()));
        }
        writeLog({});
    }

    void displayHelp() {
        writeLog(introLine, Color::Violet);
        writeLog(
            "Usage: metikoro-sim [<options>] [<n>:<agent> [<agent options>]] <backend> [<backend options>]",
            Color::Yellow);
        writeLog({});
        writeLog("Main Options:", Color::BrightWhite);
        writeLog("  --help, -h                         Display this help message");
        writeLog("  --threads=<count>, -t=<count>      Number of threads to use");
        writeLog("  --games=<count>, -g=<count>        The maximum number of games to simulate.");
        writeLog("  --version, -v                      Display version information");
        writeLog("  --no-color                         Do not use color or ANSI codes for the output.");
        writeLog("  --status-update-interval=<ms>      The interval in milliseconds for the status update.");
        writeLog("  --plain-status                     Display a simple text based status.");
        writeLog("  --console-width=<columns>          Adjust the numbers of columns for the console output.");
        writeLog({});
        writeLog(_agentRegistry.getHelp());
        writeLog(_backendRegistry.getHelp());
    }

    auto parseArguments(int argc, const char *argv[]) -> ParseResult {
        using Args = std::vector<std::string_view>;
        auto args = std::span{argv + 1, static_cast<std::size_t>(argc - 1)}
            | std::views::transform([](const char *arg) { return std::string_view{arg}; })
            | std::ranges::to<Args>();
        // First process the main options.
        for (auto it = args.begin(); it != args.end(); ++it) {
            const auto arg = *it;
            if (arg == "--help" or arg == "-h") {
                displayHelp();
                return DisplayedHelp;
            }
            if (arg == "--version" or arg == "-v") {
                std::cout << introLine << "\n";
                return DisplayedVersion;
            }
            if (arg.starts_with("--threads=") or arg.starts_with("-t=")) {
                _threads = std::stoi(std::string{arg.substr(arg.find_first_of('=') + 1)});
                _threads = std::min(std::max(_threads, static_cast<std::size_t>(1)), static_cast<std::size_t>(100));
            } else if (arg.starts_with("--games=") or arg.starts_with("-g=")) {
                _maximumGames = std::stoull(std::string{arg.substr(arg.find_first_of('=') + 1)});
            } else if (arg == "--no-color") {
                _console->setColorEnabled(false);
            } else if (arg.starts_with("--status-update-interval=")) {
                auto interval = std::stoi(std::string{arg.substr(arg.find_first_of('=') + 1)});
                interval = std::min(std::max(interval, 100), 100'000);
                _statusUpdateInterval = std::chrono::milliseconds{interval};
            } else if (arg == "--plain-status") {
                _plainStatus = true;
            } else if (arg.starts_with("--console-width=")) {
                auto width = std::stoi(std::string{arg.substr(arg.find_first_of('=') + 1)});
                width = std::min(std::max(width, 10), 1000);
                _console->setConsoleWidth(width);
            } else if (not arg.starts_with("-")) {
                args.erase(args.begin(), it);
                break;
            } else {
                throw Error{"Unknown main option: " + std::string{arg}};
            }
        }
        if (not _console->colorEnabled()) {
            _plainStatus = true;
            _statusUpdateInterval = std::max(_statusUpdateInterval, std::chrono::milliseconds{1000}); // Minimum 1s
        }
        auto optionSpan = [&args]() -> std::span<std::string_view> {
            args.erase(args.begin());
            const auto nextName = std::ranges::find_if(args, [](const auto &arg) {
                return not arg.starts_with("-");
            });
            if (nextName == args.end()) {
                return std::span{args};
            }
            return std::span{args.begin(), nextName};
        };
        auto eraseOptionSpan = [&args](const auto &span) {
            args.erase(args.begin(), args.begin() + static_cast<Args::difference_type>(span.size()));
        };
        while (not args.empty()) {
            const auto arg = args.front();
            if (arg.size() >= 3 and arg[1] == ':' and arg[0] >= '0' and arg[0] <= '3') {
                const auto name = std::string{arg.substr(2)};
                if (not _agentRegistry.hasName(name)) {
                    throw Error{"Unknown agent: " + name};
                }
                const auto index = arg[0] - '0';
                if (_agents[index] != nullptr) {
                    throw Error{"Only one agent can be specified for each player."};
                }
                _agentNames[index] = name;
                _agents[index] = _agentRegistry.create(name);
                auto agentArgs = optionSpan();
                _agents[index]->initialize(agentArgs);
                eraseOptionSpan(agentArgs);
                continue;
            }
            if (_backendRegistry.hasName(std::string{arg})) {
                if (_backend != nullptr) {
                    throw Error{"Only one backend can be specified."};
                }
                _backendName = arg;
                _backend = _backendRegistry.create(std::string{arg});
                auto backendArgs = optionSpan();
                _backend->initialize(backendArgs);
                eraseOptionSpan(backendArgs);
                continue;
            }
            throw Error{"Unknown agent or backend: " + std::string{arg}};
        }
        if (_backend == nullptr) {
            throw Error{"No backend specified."};
        }
        _backend->setConsoleWriterForwarder(_console);
        for (std::size_t i = 0; i < _agents.size(); ++i) {
            if (_agents[i] == nullptr) {
                const auto defaultName = std::string{"random"};
                _agents[i] = _agentRegistry.create(defaultName);
                _agents[i]->initialize({});
                _agentNames[i] = defaultName;
            }
            _agents[i]->setConsoleWriterForwarder(_console);
        }
        return StartSimulation;
    }

public: // attributes
    [[nodiscard]] auto backend() const noexcept -> const BackendPtr& { return _backend; }
    [[nodiscard]] auto backendName() const noexcept -> const std::string& { return _backendName; }
    [[nodiscard]] auto agentNames() const noexcept -> const std::array<std::string, Player::count>& { return _agentNames; }
    [[nodiscard]] auto agents() const noexcept -> const PlayerAgents& { return _agents; }
    [[nodiscard]] auto threads() const noexcept -> std::size_t { return _threads; }
    [[nodiscard]] auto maximumGames() const noexcept -> std::size_t { return _maximumGames; }
    [[nodiscard]] auto statusUpdateInterval() const noexcept -> std::chrono::milliseconds { return _statusUpdateInterval; }

private:
    ConsolePtr _console;
    std::chrono::milliseconds _statusUpdateInterval{250};
    bool _plainStatus{false};
    BackendRegistry _backendRegistry{};
    std::string _backendName{};
    BackendPtr _backend{};
    AgentRegistry _agentRegistry{};
    std::array<std::string, 4> _agentNames{};
    PlayerAgents _agents{};
    std::size_t _threads{16}; ///< The number of thread
    std::size_t _maximumGames{0}; ///< The maximum number of games. 0 = unlimited.
};
