#include "board.h"

const std::array<Tetromino, 7> Board::tetrominoes = createTetrominoes();

bool Board::isCollision(TetrominoType type, int rot, int x, int y) const
{
    const auto &shape = tetrominoes[static_cast<int>(type)].rotations[rot];
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c])
            {
                int gx = x + c;
                int gy = y + r;
                if (gx < 0 || gx >= COLS || gy < 0 || gy >= ROWS)
                    return true;
                if (grid[gy][gx] != 0)
                    return true;
            }
        }
    }
    return false;
}

void Board::lock(TetrominoType type, int rot, int x, int y)
{
    const auto &shape = tetrominoes[static_cast<int>(type)].rotations[rot];
    int colorVal = tetrominoes[static_cast<int>(type)].color;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c])
            {
                int gy = y + r;
                int gx = x + c;
                if (gy >= 0 && gy < ROWS && gx >= 0 && gx < COLS)
                    grid[gy][gx] = colorVal;
            }
        }
    }
}

int Board::clearLines()
{
    int cleared = 0;
    for (int r = ROWS - 1; r >= 0;)
    {
        bool full = true;
        for (int c = 0; c < COLS; ++c)
        {
            if (grid[r][c] == 0)
            {
                full = false;
                break;
            }
        }
        if (full)
        {
            for (int rr = r; rr > 0; --rr)
                grid[rr] = grid[rr - 1];
            grid[0] = {};
            ++cleared;
        }
        else
        {
            --r;
        }
    }
    return cleared;
}

bool Board::isGameOver() const
{
    for (int r = 0; r < HIDDEN_ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (grid[r][c] != 0)
                return true;
    return false;
}

void Board::reset()
{
    for (auto &row : grid)
        row.fill(0);
}

int Board::get(int row, int col) const
{
    return grid[row][col];
}