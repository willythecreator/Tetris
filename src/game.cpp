#include "game.h"
#include <algorithm>
#include <random>

Game::Game(Audio &audio) : audio(audio) { init(); }

void Game::init()
{
    board.reset();
    score = 0;
    lines = 0;
    level = 0;
    dropTimer = 0;
    lockTimer = 0;
    onGround = false;
    hasHold = false;
    holdUsed = false;
    state = GameState::Playing;

    bag.clear();
    refillBag();
    nextType = pullFromBag();
    spawnPiece();

    clearingRows.clear();
    clearTimer = 0;
    clearing = false;
}

void Game::spawnPiece()
{
    activeType = nextType;
    activeRotation = 0;
    activeX = 3;
    activeY = 0;
    lockTimer = 0;
    onGround = false;
    holdUsed = false;
    nextType = pullFromBag();

    if (board.isCollision(activeType, activeRotation, activeX, activeY))
        state = GameState::Gameover;
}

void Game::lockAndAdvance()
{
    board.lock(activeType, activeRotation, activeX, activeY);
    audio.playLock();

    // Find which rows are full
    clearingRows.clear();
    for (int r = 0; r < Board::ROWS; ++r)
    {
        bool full = true;
        for (int c = 0; c < Board::COLS; ++c)
            if (board.get(r, c) == 0)
            {
                full = false;
                break;
            }
        if (full)
            clearingRows.push_back(r);
    }

    if (!clearingRows.empty())
    {
        clearing = true;
        clearTimer = 0;
    }
    else
    {
        spawnPiece();
    }
}

// 7 bag randomizer
void Game::refillBag()
{
    static const TetrominoType all[] = {
        TetrominoType::I, TetrominoType::O, TetrominoType::T,
        TetrominoType::S, TetrominoType::Z, TetrominoType::J,
        TetrominoType::L};
    bag.assign(std::begin(all), std::end(all));

    static std::mt19937 rng(std::random_device{}());
    std::shuffle(bag.begin(), bag.end(), rng);
}

TetrominoType Game::pullFromBag()
{
    if (bag.empty())
        refillBag();
    TetrominoType t = bag.back();
    bag.pop_back();
    return t;
}

// Gravity

int Game::getDropInterval() const
{
    int ms = 800 - level * 70;
    return std::max(50, ms);
}

void Game::update(Uint32 dt)
{
    if (state != GameState::Playing)
        return;

    // Handle line clear animation
    if (clearing)
    {
        clearTimer += dt;
        if (clearTimer >= 300)
        {
            int cleared = board.clearLines();
            clearing = false;
            clearingRows.clear();

            static const int lineScores[] = {0, 100, 300, 500, 800};
            int pts = (cleared <= 4 ? lineScores[cleared] : 800) * (level + 1);
            score += pts;
            lines += cleared;
            level = lines / 10;

            if (cleared == 4)
                audio.playTetris();
            else if (cleared > 0)
                audio.playClear();
            spawnPiece();
        }
        return; // freeze everything else during animation
    }

    bool touching = board.isCollision(activeType, activeRotation, activeX, activeY + 1);

    if (touching)
    {
        lockTimer += dt;
        if (lockTimer >= 1000)
            lockAndAdvance();
    }
    else
    {
        lockTimer = 0;
        onGround = false;
        dropTimer += dt;
        if (dropTimer >= (Uint32)getDropInterval())
        {
            dropTimer = 0;
            ++activeY;
        }
    }
}

// Input

void Game::moveLeft()
{
    if (state != GameState::Playing)
        return;
    if (!board.isCollision(activeType, activeRotation, activeX - 1, activeY))
    {
        --activeX;
        audio.playMove();
        lockTimer = 0;
    }
}

void Game::moveRight()
{
    if (state != GameState::Playing)
        return;
    if (!board.isCollision(activeType, activeRotation, activeX + 1, activeY))
    {
        ++activeX;
        audio.playMove();
        lockTimer = 0;
    }
}

void Game::softDrop()
{
    if (state != GameState::Playing)
        return;
    if (!board.isCollision(activeType, activeRotation, activeX, activeY + 1))
    {
        ++activeY;
        score += 1;
        dropTimer = 0;
    }
}

void Game::hardDrop()
{
    if (state != GameState::Playing)
        return;
    while (!board.isCollision(activeType, activeRotation, activeX, activeY + 1))
    {
        ++activeY;
        score += 2;
    }
    lockAndAdvance();
}

void Game::rotateCW()
{
    if (state != GameState::Playing)
        return;

    int newRot = (activeRotation + 1) % 4;

    static const int kicks[6][2] = {
        {0, 0},
        {-1, 0},
        {1, 0},
        {-2, 0},
        {2, 0},
        {0, -1},
    };

    static const int kicksI[7][2] = {
        {0, 0},
        {-1, 0},
        {1, 0},
        {-2, 0},
        {2, 0},
        {0, -1},
        {0, -2},
    };

    bool isI = {activeType == TetrominoType::I};
    int numKicks = isI ? 7 : 6;

    for (int i = 0; i < numKicks; ++i)
    {
        int dx = isI ? kicksI[i][0] : kicks[i][0];
        int dy = isI ? kicksI[i][1] : kicks[i][1];

        if (!board.isCollision(activeType, newRot, activeX + dx, activeY + dy))
        {
            activeRotation = newRot;
            activeX += dx;
            activeY += dy;
            lockTimer = 0;
            audio.playRotate();
            return;
        }
    }
}

void Game::rotateCCW()
{
    if (state != GameState::Playing)
        return;

    int newRot = (activeRotation + 3) % 4;

    static const int kicks[6][2] = {
        {0, 0},
        {1, 0},
        {-1, 0},
        {2, 0},
        {-2, 0},
        {0, -1},
    };

    static const int kicksI[7][2] = {
        {0, 0},
        {1, 0},
        {-1, 0},
        {2, 0},
        {-2, 0},
        {0, -1},
        {0, -2},
    };

    bool isI = {activeType == TetrominoType::I};
    int numKicks = isI ? 7 : 6;

    for (int i = 0; i < numKicks; ++i)
    {
        int dx = isI ? kicksI[i][0] : kicks[i][0];
        int dy = isI ? kicksI[i][1] : kicks[i][1];

        if (!board.isCollision(activeType, newRot, activeX + dx, activeY + dy))
        {
            activeRotation = newRot;
            activeX += dx;
            activeY += dy;
            audio.playRotate();
            lockTimer = 0;
            return;
        }
    }
}

void Game::holdPiece()
{
    if (state != GameState::Playing || holdUsed)
        return;

    holdUsed = true;

    if (!hasHold)
    {
        holdType = activeType;
        hasHold = true;
        spawnPiece();
    }
    else
    {
        TetrominoType temp = activeType;
        activeType = holdType;
        holdType = temp;

        activeRotation = 0;
        activeX = 3;
        activeY = 0;
        lockTimer = 0;
        onGround = false;

        if (board.isCollision(activeType, activeRotation, activeX, activeY))
        {
            state = GameState::Gameover;
            audio.playGameOver();
        }
    }
}

void Game::togglePause()
{
    if (state == GameState::Playing)
    {
        state = GameState::Paused;
        audio.pauseMusic();
        audio.playOpenMenu();
    }
    else if (state == GameState::Paused)
    {
        state = GameState::Playing;
        audio.resumeMusic();
        audio.playExitMenu();
    }
}

// Ghost Piece

int Game::ghostY() const
{
    int y = activeY;
    while (!board.isCollision(activeType, activeRotation, activeX, y + 1))
        ++y;
    return y;
}

// Accessors

const Board &Game::getBoard() const { return board; }
TetrominoType Game::getNextPiece() const { return nextType; }
int Game::getScore() const { return score; }
int Game::getLine() const { return lines; }
int Game::getLevel() const { return level; }
GameState Game::getState() const { return state; }
TetrominoType Game::getHoldType() const { return holdType; }
bool Game::getHasHold() const { return hasHold; }
const std::vector<int> &Game::getClearingRows() const { return clearingRows; }
bool Game::isClearing() const { return clearing; }