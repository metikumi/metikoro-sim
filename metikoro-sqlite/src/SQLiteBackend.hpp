// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Backend.hpp"
#include "RatingAdjustment.hpp"
#include "GameLog.hpp"
#include "GameResult.hpp"
#include "Player.hpp"
#include "Error.hpp"

#include "sqlite3.h"

#include <condition_variable>
#include <filesystem>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>


namespace fs = std::filesystem;


class SQLiteBackend : public Backend {
    struct DbUpdate {
        std::string stateData;
        RatingAdjustment ratingAdjustment;
    };
    using DbUpdateList = std::vector<DbUpdate>;
    using DbUpdateListPtr = std::shared_ptr<DbUpdateList>;

public:
    [[nodiscard]] static auto getHelp() noexcept -> std::string {
        std::string result;
        result += "  --data-dir=<path>, -d=<path>      Path to the data directory\n";
        result += "  --cache-size=<pages>              The size of the cache in pages.\n";
        result += "  --journal-mode=<mode>             Set the journal mode for the db.\n";
        result += "  --page-size=<bytes>               The size for a page.\n";
        result += "  --synchronous-mode=<mode>         The synchronous mode.\n";
        result += "  --maximum-update-queue-size=<n>   The maximum number of update lists in the queue.\n";
        result += "  --fast-unsafe                     Set mode to WAL, sync OFF, cache 32k pages.\n";
        result += "  --vacuum                          Execute VACUUM before starting.\n";
        return result;
    }

    void initialize(std::span<std::string_view> args) override {
        for (const auto &arg : args) {
            if (arg.starts_with("--data-dir=") or arg.starts_with("-d=")) {
                _dataDir = arg.substr(arg.find_first_of('=') + 1);
            } else if (arg.starts_with("--cache-size=")) {
                auto newSize = std::stoll(std::string{arg.substr(arg.find_first_of('=') + 1)});
                if (newSize < -1'000'000 or newSize > 1'000'000) {
                    throw Error{std::format("Invalid cache size: {}", newSize)};
                }
                _cacheSize = newSize;
            } else if (arg.starts_with("--journal-mode=")) {
                constexpr auto validJournalModes = std::array{"WAL", "DELETE", "TRUNCATE", "OFF"};
                auto newMode = arg.substr(arg.find_first_of('=') + 1);
                if (std::ranges::end(validJournalModes) == std::ranges::find(validJournalModes, newMode)) {
                    throw Error{std::format("Invalid journal mode: {}", newMode)};
                }
                _journalMode = newMode;
            } else if (arg.starts_with("--page-size=")) {
                auto newSize = std::stoi(std::string{arg.substr(arg.find_first_of('=') + 1)});
                if (newSize < 1024 or newSize > 1048576) {
                    throw Error{std::format("Invalid page size: {}", newSize)};
                }
                _pageSize = newSize;
            } else if (arg.starts_with("--synchronous-mode=")) {
                constexpr auto validSynchronousModes = std::array{"OFF", "NORMAL", "FULL", "EXTRA"};
                auto newMode = arg.substr(arg.find_first_of('=') + 1);
                if (std::ranges::end(validSynchronousModes) == std::ranges::find(validSynchronousModes, newMode)) {
                    throw Error{std::format("Invalid synchronous mode: {}", newMode)};
                }
                _synchronousMode = newMode;
            } else if (arg.starts_with("--maximum-update-queue-size=")) {
                auto newSize = std::stoi(std::string{arg.substr(arg.find_first_of('=') + 1)});
                if (newSize < 1 or newSize > 10000) {
                    throw Error{std::format("Invalid maximum update queue size: {}", newSize)};
                }
                _maximumUpdateQueueSize = newSize;
            } else if (arg == "--fast-unsafe") {
                _cacheSize = 262'144; // ~1GB with 4096 page-size.
                _journalMode = "WAL";
                _synchronousMode = "OFF";
            } else if (arg == "--vacuum") {
                _executeVacuum = true;
            } else {
                throw Error{"Unknown sqlite backend option: " + std::string{arg}};
            }
        }
        if (_dataDir.empty()) {
            _dataDir = fs::current_path();
        }
        if (not fs::exists(_dataDir)) {
            throw Error{"Data directory does not exist: " + _dataDir.string()};
        }
    }

    /// Return a configuration string displayed at the start.
    ///
    void displayConfiguration() noexcept override {
        writeLog(std::format("  data-dir...................: {}", _dataDir.string()), Color::Default);
        if (_cacheSize) {
            writeLog(std::format("  cache-size.................: {}", *_cacheSize), Color::Default);
        }
        if (_journalMode) {
            writeLog(std::format("  journal-mode...............: {}", *_journalMode), Color::Default);
        }
        if (_pageSize) {
            writeLog(std::format("  page-size..................: {}", *_pageSize), Color::Default);
        }
        if (_synchronousMode) {
            writeLog(std::format("  synchronous-mode...........: {}", *_synchronousMode), Color::Default);
        }
        writeLog(std::format("  maximum-update-queue-size..: {}", _maximumUpdateQueueSize), Color::Default);
    }

    void load() override {
        _updateThread = std::async(&SQLiteBackend::databaseUpdateThread, this);
    }

    void addGame(const GameLog &gameLog) override {
        if (gameLog.empty()) {
            return;
        }
        auto ratingAdjustment = gameLog.createRatingAdjustments();
        assert(gameLog.size() == ratingAdjustment.size());
        auto updateList = std::make_shared<DbUpdateList>();
        updateList->reserve(gameLog.size());
        for (const auto &[turn, adjustment] : std::views::zip(gameLog, ratingAdjustment)) {
            updateList->emplace_back(turn.state.toData(), adjustment);
        }
        push(std::move(updateList));
    }

    [[nodiscard]] auto status() const noexcept -> std::string override {
        std::unique_lock const lock(_updateQueueMutex);
        return std::format("OK: {:> 3}/{:> 3} updates in queue.", _updateQueue.size(), _maximumUpdateQueueSize);
    }

    void shutdown() override {
        waitForQueue();
        _stopRequested = true;
        waitForUpdateThread();
    }

private:
    void waitForQueue() {
        writeLog("SQLite: Shutdown request received, waiting 10s for queue.", Color::Orange);
        const auto deadLine = std::chrono::steady_clock::now() + std::chrono::seconds{10};
        std::unique_lock lock(_updateQueueMutex);
        while (not _updateQueue.empty() and std::chrono::steady_clock::now() < deadLine) {
            constexpr std::chrono::milliseconds pollingInterval{1000}; // 1s polling interval
            _updateQueueWaitForPush.wait_for(lock, pollingInterval, [this] {
                return _updateQueue.empty();
            });
            auto remainingTimeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(deadLine - std::chrono::steady_clock::now()).count();
            writeWaitingStatus(std::format("SQLite: Waiting - {} updates in queue - {}s left.", _updateQueue.size(), remainingTimeInSeconds), Color::Orange);
        }
    }

    void waitForUpdateThread() {
        writeLog("SQLite: Waiting for update thread to finish.", Color::Orange);
        _updateQueueWaitForPush.notify_all();
        _updateQueueWaitForPop.notify_all();
        while (_updateThread.wait_for(std::chrono::seconds{1}) == std::future_status::timeout) {
            writeWaitingStatus("SQLite: Waiting for update thread to finish.", Color::Orange);
            _updateQueueWaitForPush.notify_all();
            _updateQueueWaitForPop.notify_all();
        }
        writeStatus("SQLite: Stopped.", Color::Green);
    }

    void push(DbUpdateListPtr&& updateList) {
        std::unique_lock lock(_updateQueueMutex);
        while (_updateQueue.size() >= _maximumUpdateQueueSize && not _stopRequested) {
            constexpr std::chrono::milliseconds pollingInterval{100}; // 100 ms polling interval
            _updateQueueWaitForPop.wait_for(lock, pollingInterval);
        }
        if (_stopRequested) {
            return;
        }
        _updateQueue.emplace_back(std::move(updateList));
        _updateQueueWaitForPush.notify_one();
    }

    auto popUpdateQueue() -> DbUpdateListPtr {
        std::unique_lock lock(_updateQueueMutex);
        constexpr std::chrono::milliseconds pollingInterval{100}; // 100 ms polling interval
        _updateQueueWaitForPush.wait_for(lock, pollingInterval);
        if (_stopRequested or _updateQueue.empty()) {
            return {};
        }
        auto result = _updateQueue.front(); // copy everything.
        _updateQueue.erase(_updateQueue.begin());
        _updateQueueWaitForPop.notify_all();
        return result;
    }

    void databaseUpdateThread() {
        writeLog("SQLite: Starting update thread.", Color::Green);
        openDatabase();
        displaySQLiteDetails();
        adjustPragmas();
        if (_executeVacuum) {
            callVacuum();
        }
        createSchema();
        _updateStmt = prepareUpdateStmt();
        writeLog("SQLite: Processing database updates.", Color::Green);
        while (not _stopRequested) {
            if (auto dbUpdateList = popUpdateQueue()) {
                writeUpdateList(dbUpdateList);
            }
        }
        writeLog("SQLite: Shutting down the update thread.", Color::Orange);
        _updateStmt = nullptr; // free prepared statement.
        _db = nullptr; // close database.
        writeLog("SQLite: Update thread shut down.", Color::Green);
    }

    void writeUpdateList(const DbUpdateListPtr &updateList) {
        // Start the transaction
        if (sqlite3_exec(_db.get(), "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
            throwSqliteError("Failed to begin transaction.");
        }
        try {
            for (const auto &update : *updateList) {
                auto stmt = _updateStmt.get();
                const auto &state = update.stateData;
                const auto &adj = update.ratingAdjustment;
                sqlite3_reset(stmt);
                sqlite3_bind_text(stmt, 1, state.c_str(), state.size(), SQLITE_STATIC);
                sqlite3_bind_double(stmt, 2, adj.draws());
                for (int player = 0; player < 4; ++player) {
                    sqlite3_bind_double(stmt, 3 + (player * 3), adj.ratings().at(player).combined());
                    sqlite3_bind_double(stmt, 4 + (player * 3), adj.ratings().at(player).win());
                    sqlite3_bind_double(stmt, 5 + (player * 3), adj.ratings().at(player).loss());
                }
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    throwSqliteError("Failed to execute update statement.");
                }
            }
            // Commit the transaction
            if (sqlite3_exec(_db.get(), "COMMIT TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
                throwSqliteError("Failed to commit transaction.");
            }
        } catch (const Error&) {
            // Rollback in case of an error
            sqlite3_exec(_db.get(), "ROLLBACK TRANSACTION", nullptr, nullptr, nullptr);
            throw;  // Re-throw the exception
        }
    }

    void displaySQLiteDetails() {
        writeLog(std::format("SQLite: Driver version: {}", sqlite3_libversion()), Color::Default);
    }

    void openDatabase() {
        const auto path = _dataDir / "games.db";
        sqlite3* rawDb = nullptr;
        if (sqlite3_open(path.c_str(), &rawDb) != SQLITE_OK) {
            throwSqliteError(std::format("Could not open database: \"{}\"", path.string()));
        }
        _db = std::shared_ptr<sqlite3>(rawDb, [](sqlite3* db) {
            sqlite3_close(db);
        });
    }

    void setPragma(const std::string_view &pragma, const std::string_view &value) {
        writeLog(std::format(R"(SQLite: Setting pragma "{}" to "{}")", pragma, value), Color::Default);
        if (sqlite3_exec(_db.get(), std::format("PRAGMA {} = {};", pragma, value).c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
            throwSqliteError("Failed to set pragma.");
        }
        writeLog("SQLite: done", Color::Default);
    }

    void adjustPragmas() {
        if (_cacheSize) {
            setPragma("cache_size", std::to_string(*_cacheSize));
        }
        if (_journalMode) {
            setPragma("journal_mode", *_journalMode);
        }
        if (_pageSize) {
            setPragma("page_size", std::to_string(*_pageSize));
        }
        if (_synchronousMode) {
            setPragma("synchronous", *_synchronousMode);
        }
    }

    void callVacuum() {
        writeLog("SQLite: Vacuuming database.", Color::Orange);
        if (sqlite3_exec(_db.get(), "VACUUM", nullptr, nullptr, nullptr) != SQLITE_OK) {
            throwSqliteError("Failed to vacuum database.");
        }
        writeLog("SQLite: Vacuum finished.", Color::Default);
    }

    void createSchema() {
        const auto createSchemaSQL = R"(
            CREATE TABLE IF NOT EXISTS game_state (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                state_data TEXT NOT NULL,
                game_count INTEGER,
                draws REAL,
                player0_combined REAL,
                player0_win REAL,
                player0_loss REAL,
                player1_combined REAL,
                player1_win REAL,
                player1_loss REAL,
                player2_combined REAL,
                player2_win REAL,
                player2_loss REAL,
                player3_combined REAL,
                player3_win REAL,
                player3_loss REAL
            );
            CREATE UNIQUE INDEX IF NOT EXISTS idx_game_state_data ON game_state (state_data);
            CREATE TABLE IF NOT EXISTS game_move (
                state_id INTEGER NOT NULL,
                next_move_data TEXT NOT NULL,
                next_state_id INTEGER NOT NULL
            );
            CREATE UNIQUE INDEX IF NOT EXISTS idx_game_move_id_data ON game_move (state_id, next_move_data);
        )";

        auto result = sqlite3_exec(_db.get(), createSchemaSQL, nullptr, nullptr, nullptr);
        if (result != SQLITE_OK) {
            throwSqliteError("Failed to create initial database schema.");
        }
    }

    auto prepareUpdateStmt() const -> std::shared_ptr<sqlite3_stmt> {
        sqlite3_stmt *rawStmt;
        const char updateMoveSQL[] = R"(
            INSERT INTO game_state (
                state_data, game_count, draws,
                player0_combined, player0_win, player0_loss,
                player1_combined, player1_win, player1_loss,
                player2_combined, player2_win, player2_loss,
                player3_combined, player3_win, player3_loss)
            VALUES (
                ?, 1, ?,
                ?, ?, ?,
                ?, ?, ?,
                ?, ?, ?,
                ?, ?, ?)
            ON CONFLICT (state_data)
            DO UPDATE SET
                game_count = game_count + 1,
                draws = draws + excluded.draws,
                player0_combined = player0_combined + excluded.player0_combined,
                player0_win = player0_win + excluded.player0_win,
                player0_loss = player0_loss + excluded.player0_loss,
                player1_combined = player1_combined + excluded.player1_combined,
                player1_win = player1_win + excluded.player1_win,
                player1_loss = player1_loss + excluded.player1_loss,
                player2_combined = player2_combined + excluded.player2_combined,
                player2_win = player2_win + excluded.player2_win,
                player2_loss = player2_loss + excluded.player2_loss,
                player3_combined = player3_combined + excluded.player3_combined,
                player3_win = player3_win + excluded.player3_win,
                player3_loss = player3_loss + excluded.player3_loss;
        )";
        const auto result = sqlite3_prepare_v2(_db.get(), updateMoveSQL, sizeof(updateMoveSQL), &rawStmt, nullptr);
        if (result != SQLITE_OK) {
            throwSqliteError("Failed to create update statement.");
        }
        return std::shared_ptr<sqlite3_stmt>(rawStmt, [](sqlite3_stmt *stmt) {
            sqlite3_finalize(stmt);
        });
    }

    [[noreturn]] void throwSqliteError(const std::string_view &str) const {
        auto err = std::string{sqlite3_errmsg(_db.get())};
        throw Error{std::format("{} SQLite Error: {}", str, err)};
    }

private:
    // main thread variables.
    fs::path _dataDir;
    std::size_t _maximumUpdateQueueSize{50}; ///< The maximum number of update lists in the queue, until backend blocks.
    std::optional<int64_t> _cacheSize; ///< The size of the cache in pages.
    std::optional<std::string> _journalMode; ///< The journal mode for the db.
    std::optional<std::size_t> _pageSize; ///< The size for a page.
    std::optional<std::string> _synchronousMode; ///< The synchronous mode.
    bool _executeVacuum{false}; ///< Whether to execute a VACUUM before starting.
    std::future<void> _updateThread{};

    // shared variables
    std::atomic<bool> _stopRequested{false};
    mutable std::mutex _updateQueueMutex{};
    std::condition_variable _updateQueueWaitForPush{};
    std::condition_variable _updateQueueWaitForPop{};
    std::vector<DbUpdateListPtr> _updateQueue{};

    // update thread variables.
    std::shared_ptr<sqlite3> _db{};
    std::shared_ptr<sqlite3_stmt> _updateStmt;
};

