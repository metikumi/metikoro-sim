// Copyright (c) 2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "GameState.hpp"
#include "OrbTravelNode.hpp"

#include <iostream>
#include <utility>


/// The debug interface for the move generator.
///
class OrbMoveGeneratorDebugInterface {
public:
    virtual ~OrbMoveGeneratorDebugInterface() = default;
    virtual void writeDebugMessage(const std::string &message, const GameState &state, const std::vector<OrbTravelNode> &stack) noexcept = 0;
};


/// No debugging.
///
class OrbMoveGeneratorNoDebug final : public OrbMoveGeneratorDebugInterface {
public:
    void writeDebugMessage(const std::string&, const GameState&, const std::vector<OrbTravelNode>&) noexcept override {}
};


#define ORB_MOVE_GENERATOR_DEBUG(...) \
if constexpr (debugMessages) { \
    auto message = __VA_ARGS__; \
    _debugInterface->writeDebugMessage(message, _state, _stack); \
}


/// A generator that searches for all valid orb movements in the given state.
///
template<typename DebugInterface = OrbMoveGeneratorNoDebug>
class OrbMoveGenerator {
    static constexpr bool debugMessages = not std::is_same_v<OrbMoveGeneratorDebugInterface, OrbMoveGeneratorNoDebug>;
    static constexpr auto minimumStackSize = 64;
    static constexpr auto maximumStackSize = 1024;

public:
    /// The function to add the path.
    ///
    /// @param 1 The start and stop position.
    /// @param 2 The stack at up to the stop.
    ///
    using AddPathFn = std::function<void(PositionPair, const OrbTravelNodeStack&)>;

    /// The stack type.
    ///
    using Stack = std::vector<OrbTravelNode>;

    /// The callback for debugging.
    ///
    using DebugCallback = std::function<void(const std::string&, const GameState&, const std::vector<OrbTravelNode>&)>;

public:
    /// Create a new instance of the orb move generator.
    ///
    /// @param state The state to use. It is stored as reference, make sure it exists while the generator is running!
    ///
    explicit OrbMoveGenerator(const GameState &state) : _state{state} {
        _stack.reserve(minimumStackSize);
    }

    /// Create a new instance for debugging.
    ///
    OrbMoveGenerator(const GameState &state, DebugInterface *debugInterface) : _state{state}, _debugInterface{debugInterface} {
        if (debugInterface == nullptr) {
            throw Error("OrbMoveGenerator(): debugInterface must not be null.");
        }
        _stack.reserve(minimumStackSize);
    }

    /// Get all valid orb movements for the given state.
    ///
    [[nodiscard]] auto allMoves() noexcept -> OrbMoves {
        ORB_MOVE_GENERATOR_DEBUG("allMoves()");
        OrbMoves result;
        result.add(OrbMove{}); // There is always the option to not move the orb.
        for (const auto orbPosition : _state.orbPositions().positions()) {
            const auto startPosition = orbPosition.position;
            if (startPosition.isInvalid()) {
                break; // We reached the end of the in-game orb positions.
            }
            ORB_MOVE_GENERATOR_DEBUG(std::format("start position = {}", startPosition.toString()));
            if (Board::isHouse(startPosition) && Board::playerForField(startPosition) != Player{0}) {
                ORB_MOVE_GENERATOR_DEBUG("can't move orb in house of other player.");
                continue; // Can't move an orb in the house of another player.
            }
            PositionList stopPositions;
            followAllPaths(
                startPosition,
                [&](const PositionPair &pair, const OrbTravelNodeStack &) {
                stopPositions.emplace_back(pair.second);
            });
            ORB_MOVE_GENERATOR_DEBUG(std::format("found {} valid stop positions.", stopPositions.size()));
            for (const auto stopPosition : stopPositions) {
                if (_state.orbPositions().isOrbAt(stopPosition)) {
                    continue; // Can't move an orb into another orb.
                }
                if (_state.orbPositions().koPosition(startPosition) == stopPosition) {
                    continue; // Can't move an orb back to its previous position (ko lock).
                }
                auto orbMove = OrbMove{startPosition, stopPosition};
                if (not result.contains(orbMove)) { // filter duplicates
                    result.add(orbMove);
                }
            }
        }
        ORB_MOVE_GENERATOR_DEBUG(std::format("found {} possible orb moves (including no move).", result.size()));
        return result;
    }

    /// Follow the path from the given start position and report all stops.
    ///
    /// @param startPosition The start position.
    /// @param addPathFn The function that shall be called when a stop is found.
    ///
    void followAllPaths(Position startPosition, const AddPathFn &addPathFn) noexcept {
        ORB_MOVE_GENERATOR_DEBUG(std::format("followAllPaths({}, fn)", startPosition.toString()));
        _stack.clear();
        pushNext(OrbTravelPoint{startPosition, Anchor::Stop});
        while (not _stack.empty()) {
            auto &node = _stack.back();
            ORB_MOVE_GENERATOR_DEBUG(std::format("stack size={} front={}", _stack.size(), node.toString()));
            if (node.reachedStop()) {
                ORB_MOVE_GENERATOR_DEBUG(std::format("reached stop => addPathFn(start={}, stop={}, (stack)), travel back", startPosition.toString(), node.position().toString()));
                addPathFn(PositionPair{startPosition, node.position()}, _stack);
                node.removeCurrentOption(); // this was always the last option, triggering travelBack() in every case.
            } else if (travelForward(node)) {
                continue;
            }
            if (not node.hasOptions()) {
                ORB_MOVE_GENERATOR_DEBUG("no options left => travel backward");
                travelBack(); // We can't travel forward, travel backwards instead.
            }
        }
        ORB_MOVE_GENERATOR_DEBUG("stack empty => end of method.");
    }

    /// Try to travel forward.
    ///
    /// @return `true` if we successfully traveled forward, `false` if this was no option.
    ///
    auto travelForward(OrbTravelNode &node) noexcept -> bool {
        ORB_MOVE_GENERATOR_DEBUG("travelForward()");
        if (not node.canTravelForward()) {
            if (not node.selectNextOption()) { // try to select the next option.
                return false; // no options left, can't travel forward.
            }
        }
        const auto next = node.nextPoint();
        ORB_MOVE_GENERATOR_DEBUG(std::format("next={}", next.toString()));
        if (not doesLoop(next)) {
            if (canTravel(node.position(), next.position())) { // test if the position travel is legit
                if (pushNext(next)) {
                    return true;
                }
            } else {
                ORB_MOVE_GENERATOR_DEBUG("fail: illegal travel");
            }
        } else {
            ORB_MOVE_GENERATOR_DEBUG("detected a loop, removing this option");
        }
        node.removeCurrentOption();
        return false;
    }

    auto doesLoop(const OrbTravelPoint next) noexcept -> bool {
        if (_stack.empty()) {
            return false;
        }
        return std::ranges::any_of(std::ranges::reverse_view(_stack), [&](const OrbTravelNode &n) -> bool {
            return n.beginPoint() == next;
        });
    }

    /// Try to push the next node to the stack.
    ///
    /// @return `true` if the next point was successfully pushed on the stack, `false` if there was a problem.
    ///
    auto pushNext(const OrbTravelPoint next) -> bool {
        ORB_MOVE_GENERATOR_DEBUG(std::format("pushNext({})", next.toString()));
        auto field = _state.board().field(next.position());
        ORB_MOVE_GENERATOR_DEBUG(std::format("next field = {}", field.toBoardString()));
        if (not field.empty()) {
            const auto newNode = OrbTravelNode::from(next, field);
            ORB_MOVE_GENERATOR_DEBUG(std::format("newNode = {}", newNode.toString()));
            if (not newNode.isDeadEnd()) {
                ORB_MOVE_GENERATOR_DEBUG("push new node to stack");
                if (_stack.size() >= maximumStackSize) {
                    throw Error("OrbMoveGenerator::pushNext(): stack size exceeded.");
                }
                _stack.emplace_back(newNode);
                return true;
            }
            ORB_MOVE_GENERATOR_DEBUG("fail: dead end");
        } else {
            ORB_MOVE_GENERATOR_DEBUG("fail: field empty");
        }
        return false;
    }

    void travelBack() noexcept {
        ORB_MOVE_GENERATOR_DEBUG("travelBack()");
        while (not _stack.empty()) {
            _stack.pop_back();
            if (not _stack.empty() and _stack.back().selectNextOption()) {
                break; // as soon we get another option, stop walking back.
            }
        }
    }

    /// Test if we can travel between two positions on the board.
    ///
    static auto canTravel(const Position startPosition, const Position stopPosition) noexcept -> bool {
        const auto startIsHouse = Board::isHouse(startPosition);
        const auto stopIsHouse = Board::isHouse(stopPosition);
        if (stopIsHouse and Board::playerForField(stopPosition) != Player{0}) {
            return false; // Can't move an orb in the house of another player.
        }
        if (startIsHouse and not stopIsHouse) {
            return false;
        }
        if (not Board::isSource(startPosition) and Board::isSource(stopPosition)) {
            return false;
        }
        return true;
    }

private:
    const GameState &_state;
    Stack _stack{};
    DebugInterface *_debugInterface{};
};

