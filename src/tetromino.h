#pragma once
#include <vector>
#include <array>

enum class TetrominoType
{
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

using Matrix = std::vector<std::vector<int>>;

struct Tetromino
{
    TetrominoType type;
    std::array<Matrix, 4> rotations;
    int color;
};

std::array<Tetromino, 7> createTetrominoes();