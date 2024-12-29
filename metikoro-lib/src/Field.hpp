// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Orientation.hpp"
#include "Stone.hpp"

#include <iomanip>
#include <sstream>

#include "Error.hpp"


/// One field on the game board.
class Field {
    friend struct std::hash<Field>;

protected:
    struct Data {
        uint8_t stone : 4 = 0;
        uint8_t orientation : 2 = 0;
        uint8_t koLock : 2 = 0;
    };

public:
    Field() = default;
    Field(const Stone stone, const Orientation orientation, const uint8_t koLock) noexcept : _data{
        .stone = static_cast<uint8_t>(stone),
        .orientation = static_cast<uint8_t>(stone.normalizedOrientation(orientation)),
        .koLock = std::min(static_cast<uint8_t>(3U), koLock)
    } {
    }

public:
    [[nodiscard]] constexpr auto operator==(const Field &other) const noexcept -> bool {
        return _data.stone == other._data.stone && _data.orientation == other._data.orientation && _data.koLock == other._data.koLock;
    }
    [[nodiscard]] constexpr auto operator!=(const Field &other) const noexcept -> bool { return not (*this == other); }

public:
    [[nodiscard]] auto stone() const noexcept -> Stone { return {_data.stone}; }
    [[nodiscard]] auto orientation() const noexcept -> Orientation { return static_cast<Orientation::Value>(_data.orientation); }
    void setStone(const Stone stone, const Orientation orientation) noexcept {
        _data.stone = static_cast<uint8_t>(stone);
        _data.orientation = static_cast<uint8_t>(stone.normalizedOrientation(orientation));
    }
    void setOrientation(const Orientation orientation) noexcept {
        _data.orientation = static_cast<uint8_t>(stone().normalizedOrientation(orientation));
    }
    [[nodiscard]] auto hasKoLock() const  noexcept-> bool { return _data.koLock != 0; }
    [[nodiscard]] auto koLock() const noexcept -> uint8_t { return _data.koLock; }
    void setKoLock(const uint8_t koLock) { _data.koLock = koLock; }
    [[nodiscard]] auto rotated(const Rotation rotation) const noexcept -> Field {
        if (empty() or not canRotate()) {
            return *this;
        }
        return Field{Data{
            .stone = _data.stone,
            .orientation = static_cast<uint8_t>(stone().normalizedOrientation(orientation() + rotation)),
            .koLock = _data.koLock
        }};
    }

public: // Stone wrapper
    [[nodiscard]] auto type() const noexcept -> Stone::Type { return stone().type(); }
    [[nodiscard]] auto empty() const noexcept -> bool { return stone().empty(); }
    [[nodiscard]] auto hasStop() const noexcept -> bool { return stone().hasStop();}

    [[nodiscard]] auto connectionsFrom(const Anchor connectionPoint) const -> Anchors {
        return stone().connectionsFrom(connectionPoint.normalized(orientation())).rotated(orientation());
    }
    [[nodiscard]] auto uniqueOrientations() const noexcept -> Orientations {
        return stone().uniqueOrientations();
    }
    [[nodiscard]] constexpr auto canRotate() const noexcept -> bool {
        return stone().canRotate();
    }

    [[nodiscard]] auto isValidChange(const Stone newStone, const Orientation newOrientation) const noexcept -> bool {
        if (empty() or hasKoLock()) {
            return false;
        }
        if (stone() != newStone) {
            return true;
        }
        // check if a new situation is created by this rotation.
        return not stone().isEqual(orientation(), newOrientation);
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return Stone::dataSize() + Orientation::dataSize() + 1U;
    }

    void addToData(std::string &data) const noexcept {
        if (empty()) {
            data.append(dataSize(), '_');
        } else {
            stone().addToData(data);
            orientation().addToData(data);
            if (hasKoLock()) {
                data += utility::valueToHexDigit(koLock());
            } else {
                data += '_';
            }
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> Field {
        if (data.size() != dataSize()) {
            throw Error("Field: Invalid data size.");
        }
        if (data[0] == '_') {
            return Field{}; // empty field.
        }
        return {
            Stone::fromData(data.substr(0, Stone::dataSize())),
            Orientation::fromData(data.substr(Stone::dataSize(), Orientation::dataSize())),
            utility::hexDigitToValue(data[Stone::dataSize() + Orientation::dataSize()])};
        // please note: this relies on the fact, that `hexDigitToValue` will convert `_` into zero!
    }

public: // display
    [[nodiscard]] auto toBoardString() const noexcept -> std::string {
        if (empty()) {
            return "  ";
        }
        return stone().toString() + orientation().toArrow();
    }

    [[nodiscard]] auto toDebugString() const noexcept -> std::string {
        std::stringstream result;
        result << "data.stone = 0x" << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<uint32_t>(_data.stone) << " (" << stone().toString() << ")";
        result << ", data.rotation = 0x" << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<uint32_t>(_data.orientation) << " (" << orientation().toArrow() << ")";
        result << ", data.koLock = 0x" << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<uint32_t>(_data.koLock);
        return result.str();
    }

public: // Gameplay
    void nextTurn() noexcept {
        if (hasKoLock()) {
            setKoLock(koLock() - 1);
        }
    }

public:
    explicit Field(const Data data) : _data{data} {};

protected:
    [[nodiscard]] auto data() const -> const Data& { return _data; }

protected:
    Data _data{};
};


static_assert(Serializable<Field>);


template<>
struct std::hash<Field> {
    auto operator()(const Field &field) const noexcept -> std::size_t {
        return utility::hashFromValues(field._data.stone, field._data.orientation, field._data.koLock);
    }
};


