cmake_minimum_required(VERSION 3.22)
project(unittest)
add_executable(unittest
        src/main.cpp
        src/AnchorTest.cpp
        src/AnchorsTest.cpp
        src/StoneWiringTest.cpp
        src/RotationTest.cpp
        src/OrientationsTest.cpp
        src/GameStateTest.cpp
        src/OrientationTest.cpp
        src/StonePoolTest.cpp
        src/OrbMoveGeneratorTest.cpp
        src/FieldTest.cpp
        src/BoardTest.cpp
        src/UtilitiesTest.cpp)
target_link_libraries(unittest PRIVATE metikoro-lib)
target_include_directories(unittest PRIVATE ../metikoro-lib/src)
erbsland_unittest(TARGET unittest)