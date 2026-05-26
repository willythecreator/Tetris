#pragma once
#include "board.h"
#include <SDL2/SDL.h>
#include <vector>

enum class GameState
{
    Playing,
    Paused,
    Gameover
};

class Game
{
public:
    Game();

    void init();
    void update(Uint32 dt);

    // Input actions
    void moveLeft();
    void moveRight();
    void softDrop();
    void hardDrop();
    void rotateCW();
    void togglePause();

    // Accessors
    const Board &getBoard() const;
    TetrominoType getNextPiece() const;
    int getScore() const;
    int getLine() const;
    int getLevel() const;
    GameState getState() const;

private:
    Board board;

    // Active piece
    TetrominoType activeType;
    int activeX, activeY;
    int activeRotation;

    // Next piece preview
    TetrominoType nextType;

    // 7-bag randomizer
    std::vector<TetrominoType> bag;
    void refillBag();
    TetrominoType pullFromBag();

    // Gravity
    Uint32 dropTimer;
    int getDropInterval() const;

    // Scoring
    int score;
    int lines;
    int level;

    GameState state;

    // Helpers
    void spawnPiece();
    void lockAndAdvance();
    int ghostY() const;
};