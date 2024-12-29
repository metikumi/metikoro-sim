// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "ConsoleWriter.hpp"
#include "GameLog.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <ranges>
#include <vector>


class Backend;
using BackendPtr = std::shared_ptr<Backend>;


class Backend : public ConsoleWriter {
public:
    ~Backend() override = default;

public:
    /// Display help.
    ///
    [[nodiscard]] static auto getHelp() noexcept -> std::string {
        return {}; // Implement this in the derived class to display help.
    }

    /// Initialize the storage.
    ///
    /// Called once after creating the storage object.
    ///
    /// @param args The arguments for this backend.
    /// @throws Error in case there was a problem with the command line arguments.
    ///
    virtual void initialize(std::span<std::string_view> args) = 0;

    /// Display the configuration for this backend.
    ///
    virtual void displayConfiguration() noexcept {}

    /// Loading previous data into the storage.
    ///
    /// Called after initializing the object.
    ///
    virtual void load() = 0;

    /// Adding the game log of a finished game.
    ///
    /// @warning This method must be thread safe! It is called from the simulation threads.
    ///
    virtual void addGame(const GameLog &gameLog) = 0;

    /// Return the status of the backend.
    ///
    /// @warning The call of this method must be thread safe.
    ///
    [[nodiscard]] virtual auto status() const noexcept -> std::string {
        return "OK";
    }

    /// Shutdown the storage.
    ///
    /// Called when the user stops of the program from the main thread. The method is called
    /// after all simulation threads stop.
    ///
    virtual void shutdown() = 0;
};

