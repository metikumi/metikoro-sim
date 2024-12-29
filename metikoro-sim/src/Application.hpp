// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Configuration.hpp"
#include "GameSimulator.hpp"
#include "Console.hpp"
#include "RollingAverage.hpp"

#include <atomic>
#include <complex>
#include <csignal>
#include <future>
#include <memory>
#include <vector>


class Application final : public ConsoleWriter {
public:
    /// The maximum number of rolling average values to consider.
    constexpr static auto rollingAverageCount = 100;

public:
    Application() : _console(std::make_shared<Console>()), _configuration(_console) {
        assert(_instance == nullptr);
        _instance = this;
        setConsoleWriterForwarder(_console);
    }

    ~Application() override {
        _instance = nullptr;
    }

    auto run(const int argc, const char *argv[]) -> int {
        try {
            if (_configuration.parseArguments(argc, argv) != Configuration::StartSimulation) {
                return 0;
            }
            _configuration.displayIntro();
            registerSignals();
            loadBackend();
            startSimulationThreads();
            startSimulationStatusThread();
            waitForSimulationEnd();
            shutdownBackend();
            return 0;
        } catch (const Error &error) {
            writeLog({});
            writeLog(std::format("*** ERROR: {} ***", error.what()), Color::Red);
            writeLog({});
            _configuration.displayHelp();
            return 1;
        }
    }

    void registerSignals() {
        signal(SIGINT, &stopRequested); // Register signal handler for SIGINT (Ctrl+C)
        signal(SIGUSR1, &stopRequested); // Register signal handler for SIGUSR1 for use while debugging.
    }

    static void stopRequested(int /*status*/) {
        if (_instance == nullptr) {
            return;
        }
        _instance->_console->writeLog(
            "\n*** Termination signal received. Stopping simulation... ***\n", Color::Red);
        _stopRequested = true;
    }

    void loadBackend() {
        writeStatus(std::format("Backend {}: loading data...", _configuration.backendName()), Color::Orange);
        _configuration.backend()->load();
    }

    void startSimulationThreads() {
        writeStatus("Starting simulation...", Color::Yellow);
        _simulationRunning = std::make_unique<std::atomic_bool[]>(_configuration.threads());
        _simulationFutures.reserve(_configuration.threads());
        for (std::size_t i = 0; i < _configuration.threads(); ++i) {
            _simulationRunning[i] = false;
            _simulationFutures.push_back(std::async(
                &Application::simulationThread, this, i, &_simulationRunning[i]));
        }
        writeWaitingStatus("Waiting for the simulation threads to start", Color::Yellow);
        while (not (isSimulationStopped() or allSimulationsStarted())) {
            writeWaitingStatus("Waiting for the simulation threads to start", Color::Yellow);
            std::this_thread::sleep_for(_configuration.statusUpdateInterval());
        }
        writeStatus("Simulation started.", Color::Green);
    }

    [[nodiscard]] auto allSimulationsStarted() const noexcept -> bool {
        for (std::size_t i = 0; i < _configuration.threads(); ++i) {
            if (not _simulationRunning[i]) {
                return false;
            }
        }
        return true;
    }

    void simulationThread(const std::size_t threadId, std::atomic_bool *runningFlag) {
        assert(runningFlag != nullptr);
        writeStatus(std::format("Simulation thread {}: started.", threadId), Color::LightBlue);
        PlayerAgents agents{};
        const auto &configuredAgents = _configuration.agents();
        for (std::size_t i = 0; i < configuredAgents.size(); ++i) {
            agents[i] = configuredAgents[i]->copyForThread();
        }
        *runningFlag = true;
        while (not isSimulationStopped()) {
            simulateGame(agents);
        }
        writeStatus(std::format("Simulation thread {}: shutting down agent...", threadId), Color::LightBlue);
        for (const auto &agent : agents) {
            agent->shutdown();
        }
        writeStatus(std::format("Simulation thread {}: stopped.", threadId), Color::LightBlue);
    }

    void simulateGame(const PlayerAgents &agents) noexcept {
        for (const auto &agent : agents) {
            agent->gameStart();
        }
        auto gameSimulator = GameSimulator(agents);
        gameSimulator.run();
        for (const auto &agent : agents) {
            agent->gameEnd(gameSimulator.gameLog());
        }
        _configuration.backend()->addGame(gameSimulator.gameLog());
        addGameStat(gameSimulator.gameLog());
    }

    void startSimulationStatusThread() {
        _statusUpdateFuture = std::async(&Application::simulationStatusThread, this);
    }

    void simulationStatusThread() {
        while (not isSimulationStopped()) {
            displaySimulationStatus();
            std::this_thread::sleep_for(_configuration.statusUpdateInterval());
        }
    }

    void displaySimulationStatus() {
        using namespace std::chrono;
        static steady_clock::time_point lastDisplay{};

        if (isSimulationStopped()) {
            return;
        }
        std::unique_lock const lock(_statMutex);
        const auto now = steady_clock::now();
        const auto duration = now - std::exchange(lastDisplay, now);

        const auto gamesInDuration = _simulationRating.ratingCount() - _lastSimulatedGamesCount;
        _lastSimulatedGamesCount = _simulationRating.ratingCount();
        const auto gamesPerHour = static_cast<double>(gamesInDuration) /
            static_cast<double>(duration_cast<milliseconds>(duration).count()) * 3'600'000.0;

        _gamesPerHour.add(gamesPerHour);
        if (_console->colorEnabled()) {
            _console->writeSimulationStatus(
                _simulationRating,
                _gamesPerHour.average(),
                _moveAverage.average(),
                _configuration.backend()->status());
        } else {
            writeStatus(std::format("Simulation Running: {}", _simulationRating.toString()), Color::Green);
        }
    }

    void addGameStat(const GameLog &gameLog) {
        std::unique_lock const lock(_statMutex);
        _simulationRating.applyAdjustment(RatingAdjustment{gameLog.winningPlayer()});
        _moveAverage.add(static_cast<double>(gameLog.size()));
        if (hasMaximumGamesReached()) {
            _stopRequested = true; // As soon we reach a configured maximum of games, kindly request a stop.
        }
    }

    void waitForSimulationEnd() {
        while (not isSimulationStopped()) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
        bool waitingForFuture = true;
        while (waitingForFuture) {
            auto waitUntil = std::chrono::steady_clock::now() + _configuration.statusUpdateInterval();
            waitingForFuture = false;
            for (const std::future<void> &future : _simulationFutures) {
                if (future.wait_until(waitUntil) == std::future_status::timeout) {
                    writeWaitingStatus("Waiting for simulation threads to finish.", Color::Yellow);
                    waitingForFuture = true;
                    break;
                }
            }
        }
    }

    [[nodiscard]] static auto isSimulationStopped() noexcept -> bool {
        return _stopRequested;
    }

    [[nodiscard]] auto hasMaximumGamesReached() const noexcept -> bool {
        return _configuration.maximumGames() > 0 and _simulationRating.ratingCount() >= _configuration.maximumGames();
    }

    void shutdownBackend() {
        writeWaitingStatus("All simulation threads stopped, shutting down backend...", Color::Yellow);
        _configuration.backend()->shutdown();
        writeLog("Simulation stopped.", Color::Green);
    }

private:
    static inline Application *_instance{nullptr};
    static inline std::atomic_bool _stopRequested{false};

    ConsolePtr _console;
    Configuration _configuration;

    std::future<void> _statusUpdateFuture;
    std::vector<std::future<void>> _simulationFutures;
    std::unique_ptr<std::atomic_bool[]> _simulationRunning;
    std::mutex _statMutex;
    RollingAverage<double, rollingAverageCount> _gamesPerHour;
    RollingAverage<double, rollingAverageCount> _moveAverage;
    uint64_t _lastSimulatedGamesCount{0};
    RatingGame _simulationRating;
};
