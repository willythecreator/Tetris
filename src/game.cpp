#include "game.h"
#include <algorithm>
#include <random>

Game::Game() { init(); }

void Game::init()
{
    board.reset();
    score = 0;
    lines = 0;
    level = 0;
    dropTimer = 0;
    state = GameState::Playing;

    bag.clear();
    refillBag();
    nextType = pullFromBag();
    spawnPiece();
}

void Game::spawnPiece()
{
    activeType = nextType;
    activeRotation = 0;
    activeX = 3;
    activeY = 0;
    nextType = pullFromBag();

    if (board.isCollision(activeType, activeRotation, activeX, activeY))
        state = GameState::Gameover;
}

void Game::lockAndAdvance()
{
    board.lock(activeType, activeRotation, activeX, activeY);
    int cleared = board.clearLines();

    if (cleared > 0)
    {
        static const int lineScores[] = {0, 100, 300, 500, 800};
        int pts = (cleared <= 4 ? lineScores[cleared] : 800) * (level + 1);
        score += pts;
        lines += cleared;
        level = lines / 10;
    }

    spawnPiece();
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

    dropTimer += dt;
    if (dropTimer >= getDropInterval())
    {
        dropTimer = 0;
        if (board.isCollision(activeType, activeRotation, activeX, activeY + 1))
            lockAndAdvance();
        else
            ++activeY;
    }
}

// Input

void Game::moveLeft()
{
    if (state != GameState::Playing)
        return;
    if (!board.isCollision(activeType, activeRotation, activeX - 1, activeY))
        --activeX;
}

void Game::moveRight()
{
    if (state != GameState::Playing)
        return;
    if (!board.isCollision(activeType, activeRotation, activeX + 1, activeY))
        ++activeX;
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
    if (!board.isCollision(activeType, newRot, activeX, activeY))
        activeRotation = newRot;
}

void Game::togglePause()
{
    if (state == GameState::Playing)
        state = GameState::Paused;
    else if (state == GameState::Paused)
        state = GameState::Playing;
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