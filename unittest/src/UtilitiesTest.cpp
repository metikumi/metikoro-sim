// Copyright (c) 2024-2025 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later


#include <erbsland/unittest/UnitTest.hpp>

#include "Utilities.hpp"


class UtilitiesTest : public el::UnitTest {
public:
    void testSizeUTF8() {
        auto size = utility::sizeUTF8("");
        REQUIRE(size == 0);
        size = utility::sizeUTF8("a");
        REQUIRE(size == 1);
        size = utility::sizeUTF8("ab");
        REQUIRE(size == 2);
        size = utility::sizeUTF8("●");
        REQUIRE(size == 1);
        size = utility::sizeUTF8("a●b");
        REQUIRE(size == 3);
        size = utility::sizeUTF8("a▲b");
        REQUIRE(size == 3);
        size = utility::sizeUTF8("ab⬛");
        REQUIRE(size == 3);
    }
};


