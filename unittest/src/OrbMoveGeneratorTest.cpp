// Copyright (c) 2024-2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "GameState.hpp"
#include "OrbMoveGenerator.hpp"



#define GENERATOR_SETUP() OrbMoveGenerator gen(state, this);


class OrbMoveGeneratorTest : public el::UnitTest, public OrbMoveGeneratorDebugInterface {
public:
    GameState state;
    std::vector<std::string> debugMessages;

    auto additionalErrorMessages() -> std::string override {
        try {
            std::stringstream result;
            result << "State:\n" << state.toString() << "\n";
            result << "Debug Messages:\n";
            for (const auto& message : debugMessages) {
                result << message << "\n";
            }
            return result.str();
        } catch (...) {
            return {};
        }
    }

    void setUp() override {
        debugMessages.clear();
    }

    void writeDebugMessage(
        const std::string& message,
        const GameState& /*state*/,
        const std::vector<OrbTravelNode>& /*stack*/) noexcept override {
        debugMessages.push_back(message);
    }

    void testDefault() {
        auto state = GameState::createStartingGameState();
        GENERATOR_SETUP();
        auto moves = gen.allMoves();
        REQUIRE(moves.size() == 1);
        REQUIRE(moves[0].isNoMove());
    }

    void testOneStop() {
        state = GameState::createStartingGameState();
        state.board().setField(Position{3, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{2, 4}, Stone::TwoCurves, Orientation::North);
        state.board().setField(Position{2, 3}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{2, 2}, Stone::CrossingWithStop, Orientation::North);
        GENERATOR_SETUP();
        auto moves = gen.allMoves();
        REQUIRE(moves.size() == 2);
        REQUIRE(moves[0].isNoMove());
        REQUIRE(moves[1].start() == Position{4, 4});
        REQUIRE(moves[1].stop() == Position{2, 2});
    }

    void testManyStops() {
        state = GameState::createStartingGameState();
        state.board().setField(Position{3, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{2, 4}, Stone::TwoCurves, Orientation::North);
        state.board().setField(Position{2, 3}, Stone::SwitchB, Orientation::South);
        state.board().setField(Position{2, 2}, Stone::CrossingWithStop, Orientation::North);
        state.board().setField(Position{1, 3}, Stone::CrossingWithStop, Orientation::North);
        state.board().setField(Position{3, 3}, Stone::CrossingWithStop, Orientation::North);
        state.board().setField(Position{4, 3}, Stone::SwitchA, Orientation::North);
        state.board().setField(Position{4, 2}, Stone::CrossingWithStop, Orientation::North);
        state.board().setField(Position{5, 3}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{5, 2}, Stone::TwoCurves, Orientation::North);
        state.board().setField(Position{6, 4}, Stone::CrossingWithStop, Orientation::North);
        GENERATOR_SETUP();
        auto moves = gen.allMoves();
        REQUIRE(moves.size() == 7);
        REQUIRE(moves[0].isNoMove());
        // accept any order of the result.
        REQUIRE(moves.contains(OrbMove{Position{4, 4}, Position{2, 2}}));
        REQUIRE(moves.contains(OrbMove{Position{4, 4}, Position{1, 3}}));
        REQUIRE(moves.contains(OrbMove{Position{4, 4}, Position{3, 3}}));
        REQUIRE(moves.contains(OrbMove{Position{4, 4}, Position{4, 2}}));
        REQUIRE(moves.contains(OrbMove{Position{5, 4}, Position{4, 2}}));
        REQUIRE(moves.contains(OrbMove{Position{5, 4}, Position{6, 4}}));
    }

    void testDeadEnd() {
        state = GameState::createStartingGameState();
        state.board().setField(Position{3, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{2, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{4, 3}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{4, 2}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{4, 1}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{3, 5}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{1, 5}, Stone::CrossingWithStop, Orientation::North);
        GENERATOR_SETUP();
        auto moves = gen.allMoves();
        REQUIRE(moves.size() == 1);
        REQUIRE(moves[0].isNoMove());
    }

    void testMoveBackToSource() {
        state = GameState::createStartingGameState();
        state.board().setField(Position{3, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{2, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{1, 4}, Stone::CrossingWithStop, Orientation::North);
        state.orbPositions().moveOrb(Position{4, 4}, Position{1, 4});
        state.orbPositions().nextTurn(); // remove any ko locks
        state.orbPositions().nextTurn(); // remove any ko locks
        state.orbPositions().nextTurn(); // remove any ko locks
        state.orbPositions().nextTurn(); // remove any ko locks
        GENERATOR_SETUP();
        auto moves = gen.allMoves();
        REQUIRE(moves.size() == 1);
        REQUIRE(moves[0].isNoMove());
    }

    void testMoveAwayFromHome() {
        state = GameState::createStartingGameState();
        state.board().setField(Position{1, 1}, Stone::CrossingWithStop, Orientation::North);
        state.board().setField(Position{2, 1}, Stone::CrossingWithStop, Orientation::North);
        state.board().setField(Position{1, 2}, Stone::CrossingWithStop, Orientation::North);
        state.orbPositions().moveOrb(Position{4, 4}, Position{0, 0});
        state.orbPositions().moveOrb(Position{5, 4}, Position{1, 0});
        state.orbPositions().moveOrb(Position{4, 5}, Position{0, 1});
        GENERATOR_SETUP();
        auto moves = gen.allMoves();
        REQUIRE(moves.size() == 1);
        REQUIRE(moves[0].isNoMove());
    }

    void testLoop() {
        state = GameState::createStartingGameState();
        state.board().setField(Position{3, 4}, Stone::SwitchA, Orientation::West);
        state.board().setField(Position{2, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{1, 4}, Stone::TwoCurves, Orientation::North);
        state.board().setField(Position{1, 3}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{1, 2}, Stone::TwoCurves, Orientation::East);
        state.board().setField(Position{2, 2}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{3, 2}, Stone::TwoCurves, Orientation::North);
        state.board().setField(Position{3, 3}, Stone::Crossing, Orientation::North);
        GENERATOR_SETUP();
        auto moves = gen.allMoves();
        REQUIRE(moves.size() == 1);
        REQUIRE(moves[0].isNoMove());
    }
};