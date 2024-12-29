// Copyright (c) 2024 Metikumi. https://metikumi.com
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


#include "Position.hpp"
#include "Rotation.hpp"
#include "Stone.hpp"

#include <array>


template<typename FieldType, std::size_t N>
class FieldGrid {
public:
    static constexpr std::size_t fieldCount = N * N;
    using FieldArray = std::array<FieldType, fieldCount>;
    static constexpr std::size_t sideLength = N;

public:
    FieldGrid() = default;

public:
    [[nodiscard]] auto operator==(const FieldGrid &other) const noexcept -> bool { return _fields == other._fields; }
    [[nodiscard]] auto operator!=(const FieldGrid &other) const noexcept -> bool { return not (*this == other); }

public:
    [[nodiscard]] auto fields() const noexcept -> const FieldArray& { return _fields; }
    [[nodiscard]] auto field(const Position position) const -> FieldType {
        if (position.x() >= sideLength) {
            throw Error("FieldGrid: X position out of bounds.");
        }
        if (position.y() >= sideLength) {
            throw Error("FieldGrid: Y position out of bounds.");
        }
        return _fields.at(position.y() * sideLength + position.x());
    }
    [[nodiscard]] auto field(const Position position) -> FieldType& {
        if (position.x() >= sideLength) {
            throw Error("FieldGrid: X position out of bounds.");
        }
        if (position.y() >= sideLength) {
            throw Error("FieldGrid: Y position out of bounds.");
        }
        return _fields.at(position.y() * sideLength + position.x());
    }
    void setField(const Position position, const Stone stone, const Orientation orientation) noexcept {
        field(position).setStone(stone, orientation);
    }
    void setFieldRotated(const Position position, const Stone stone, const Orientation orientation, const Rotation rotation) noexcept {
        field(rotatedPosition(rotation, position)).setStone(stone, orientation - rotation);
    }
    void rotateField(const Position position, const Rotation rotation) {
        field(position).rotate(rotation);
    }
    [[nodiscard]] auto rotated(const Rotation rotation) const -> FieldGrid {
        FieldGrid result{};
        for (Length y = 0; y < sideLength; ++y) {
            for (Length x = 0; x < sideLength; ++x) {
                const auto sourcePosition = Position{x, y};
                const auto targetPosition = rotatedPosition(rotation, sourcePosition);
                result.field(targetPosition) = field(sourcePosition).rotated(rotation);
            }
        }
        return result;
    }

public: // serialization
    [[nodiscard]] constexpr static auto dataSize() noexcept -> std::size_t {
        return FieldType::dataSize() * fieldCount;
    }

    void addToData(std::string &data) const noexcept {
        for (const auto field : _fields) {
            field.addToData(data);
        }
    }

    [[nodiscard]] static auto fromData(const std::string_view &data) -> FieldGrid {
        if (data.size() != dataSize()) {
            throw Error("FieldGrid: Invalid data size.");
        }
        FieldGrid result{};
        for (Length i = 0; i < fieldCount; ++i) {
            result._fields.at(i) = FieldType::fromData(data.substr(i * FieldType::dataSize(), FieldType::dataSize()));
        }
        return result;
    }

public:
    [[nodiscard]] static auto rotatedPosition(const Rotation rotation, const Position position) -> Position {
        return position.rotated(rotation, N);
    }

private:
    FieldArray _fields{};
};


template<typename FieldType, std::size_t N>
struct std::hash<FieldGrid<FieldType, N>> {
    auto operator()(const FieldGrid<FieldType, N> &fieldGrid) const noexcept -> std::size_t {
        return utility::hashFromArray(fieldGrid.fields());
    }
};

