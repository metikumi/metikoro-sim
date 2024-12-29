// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include <Agent.hpp>

#include <map>
#include <memory>
#include <sstream>
#include <string>


class AgentBuilder {
public:
    virtual ~AgentBuilder() = default;
    [[nodiscard]] virtual auto getHelp() noexcept -> std::string = 0;
    virtual auto create() noexcept -> AgentPtr = 0;
};

using AgentBuilderPtr = std::shared_ptr<AgentBuilder>;

template<typename T>
class AgentBuilderImpl : public AgentBuilder {
public:
    [[nodiscard]] auto getHelp() noexcept -> std::string override {
        return T::getHelp();
    }
    auto create() noexcept -> AgentPtr override {
        return std::make_shared<T>();
    }
};


class AgentRegistry {
public:
    AgentRegistry() = default;

public:
    template<typename T>
    void add(const std::string &name) noexcept {
        assert(not _builders.contains(name));
        _builders[name] = std::make_shared<AgentBuilderImpl<T>>();
    }

    [[nodiscard]] auto hasName(const std::string &name) const noexcept -> bool {
        return _builders.contains(name);
    }

    [[nodiscard]] auto names() const noexcept -> std::vector<std::string> {
        return _builders | std::views::keys | std::ranges::to<std::vector>();
    }

    [[nodiscard]] auto create(const std::string &name) const noexcept -> AgentPtr {
        assert(_builders.contains(name));
        return _builders.at(name)->create();
    }

    [[nodiscard]] auto getHelp() noexcept -> std::string {
        std::stringstream result;
        for (const auto &[name, builder] : _builders) {
            result << "Options for Agent \"" << name << "\":\n";
            auto help = builder->getHelp();
            if (not help.empty()) {
                result << builder->getHelp() << "\n";
            }
        }
        return result.str();
    }

private:
    std::map<std::string, AgentBuilderPtr> _builders;
};

