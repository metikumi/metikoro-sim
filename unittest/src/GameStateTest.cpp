// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "GameState.hpp"

#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>


class GameStateTest : public el::UnitTest {
public:
    GameState state;

    auto additionalErrorMessages() -> std::string override {
        try {
            std::stringstream result;
            result << "State:\n" << state.toString() << "\n";
            return result.str();
        } catch (...) {
            return {};
        }
    }

    SKIP_BY_DEFAULT()
    void testInitialMoveUniqueness() {
        state = GameState::createStartingGameState();
        std::unordered_set<GameMove> moves;
        std::unordered_map<GameState, GameMove> followingStates;
        for (auto move : state.allMoves()) {
            REQUIRE_FALSE(moves.contains(move));
            moves.insert(move);
            auto followingState = state.afterMove(move);
            if (followingStates.contains(followingState)) {
                std::stringstream result;
                result << "Duplicate move: " << move.toString() << "\n";
                result << "  like previous: " << followingStates[followingState].toString() << "\n";
                consoleWriteLine(result.str());
            }
            REQUIRE_FALSE(followingStates.contains(followingState));
            followingStates[followingState] = move;
        }
    }

    void testOrbsInHouse() {
        state = GameState::createStartingGameState();
        REQUIRE(state.orbsInHouse() == std::array<uint8_t, 4>{0, 0, 0, 0});
        state.orbPositions().moveOrb(Position::invalid(), Position{0, 0});
        REQUIRE(state.orbsInHouse() == std::array<uint8_t, 4>{1, 0, 0, 0});
        state.orbPositions().moveOrb(Position::invalid(), Position{1, 0});
        REQUIRE(state.orbsInHouse() == std::array<uint8_t, 4>{2, 0, 0, 0});
        state.orbPositions().moveOrb(Position::invalid(), Position{0, 1});
        REQUIRE(state.orbsInHouse() == std::array<uint8_t, 4>{3, 0, 0, 0});

        state = GameState::createStartingGameState();
        state.orbPositions().moveOrb(Position::invalid(), Position{setup::boardSize - 1, 0});
        REQUIRE(state.orbsInHouse() == std::array<uint8_t, 4>{0, 0, 0, 1});
        state.orbPositions().moveOrb(Position::invalid(), Position{setup::boardSize - 2, 0});
        REQUIRE(state.orbsInHouse() == std::array<uint8_t, 4>{0, 0, 0, 2});
        state.orbPositions().moveOrb(Position::invalid(), Position{setup::boardSize - 1, 1});
        REQUIRE(state.orbsInHouse() == std::array<uint8_t, 4>{0, 0, 0, 3});
    }

    void testSerialization() {
        // Set up a board
        state = GameState::createStartingGameState();
        state.board().setField(Position{3, 4}, Stone::SwitchA, Orientation::West);
        state.board().setField(Position{2, 4}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{1, 4}, Stone::TwoCurves, Orientation::North);
        state.board().setField(Position{1, 3}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{1, 2}, Stone::TwoCurves, Orientation::East);
        state.board().setField(Position{2, 2}, Stone::Crossing, Orientation::North);
        state.board().setField(Position{3, 2}, Stone::TwoCurves, Orientation::North);
        state.board().setField(Position{3, 3}, Stone::Crossing, Orientation::North);
        state.executeMove(
            GameMove{
                ActionSequence{
                    {
                        Action::createPlace(Position{6, 4}, Stone::Crossing, Orientation::North),
                        Action::createPlace(Position{6, 5}, Stone::Crossing, Orientation::North)
                    }
                },
                Stone::SwitchA,
                OrbMove{}
            }
        );
        state.orbPositions().moveOrb(Position::invalid(), Position{0, 0});
        state.orbPositions().moveOrb(Position::invalid(), Position{1, 0});
        state = state.rotated(Rotation::Clockwise270);
        // ---
        auto data = state.toData();
        REQUIRE(data.size() == GameState::dataSize());
        REQUIRE(data.substr(0, 3U) == "S1:");
        auto deserializedState = GameState::fromData(data);
        REQUIRE(deserializedState.toData() == data);
        REQUIRE(deserializedState == state);

        state.board().setField(Position{6, 6}, Stone::Crossing, Orientation::North);
        REQUIRE(data != state.toData());
    }
};


