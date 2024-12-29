// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <map>

#include "Backend.hpp"

#include <cassert>
#include <memory>
#include <ranges>


class BackendBuilder {
public:
    virtual ~BackendBuilder() = default;
    [[nodiscard]] virtual auto getHelp() noexcept -> std::string = 0;
    virtual auto create() noexcept -> BackendPtr = 0;
};
using BackendBuilderPtr = std::shared_ptr<BackendBuilder>;


template<typename T>
class BackendBuilderImpl : public BackendBuilder {
public:
    [[nodiscard]] auto getHelp() noexcept -> std::string override {
        return T::getHelp();
    }
    auto create() noexcept -> BackendPtr override {
        return std::make_shared<T>();
    }
};


class BackendRegistry {
public:
    BackendRegistry() = default;

public:
    template<typename T>
    void add(const std::string &name) noexcept {
        assert(not _builders.contains(name));
        _builders[name] = std::make_shared<BackendBuilderImpl<T>>();
    }

    [[nodiscard]] auto hasName(const std::string &name) const noexcept -> bool {
        return _builders.contains(name);
    }

    [[nodiscard]] auto names() const noexcept -> std::vector<std::string> {
        return _builders | std::views::keys | std::ranges::to<std::vector>();
    }

    [[nodiscard]] auto create(const std::string &name) const noexcept -> BackendPtr {
        assert(_builders.contains(name));
        return _builders.at(name)->create();
    }

    [[nodiscard]] auto getHelp() noexcept -> std::string {
        std::stringstream result;
        for (const auto &[name, builder] : _builders) {
            result << "Options for Backend \"" << name << "\":\n";
            auto help = builder->getHelp();
            if (not help.empty()) {
                result << builder->getHelp() << "\n";
            }
            result << "\n";
        }
        return result.str();
    }

public:
    std::map<std::string, BackendBuilderPtr> _builders{};
};

