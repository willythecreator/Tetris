#pragma once
#include <vector>
#include "tetromino.h"

class Board
{
public:
    static constexpr int ROWS = 22; // 20 visible + 2 hidden above
    static constexpr int COLS = 10;
    static constexpr int HIDDEN_ROWS = 2;

    Board() = default;

    bool isCollision(TetrominoType type, int rot, int x, int y) const;
    void lock(TetrominoType type, int rot, int x, int y);
    int clearLines();
    bool isGameOver() const;
    void reset();

    int get(int row, int col) const;

private:
    std::array<std::array<int, COLS>, ROWS> grid{};
    static const std::array<Tetromino, 7> tetrominoes;
};