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
    void holdPiece();
    void rotateCCW();

    // Accessors
    const Board &getBoard() const;
    TetrominoType getNextPiece() const;
    int getScore() const;
    int getLine() const;
    int getLevel() const;
    int ghostY() const;
    GameState getState() const;

    TetrominoType getActiveType() const { return activeType; }
    int getActiveX() const { return activeX; }
    int getActiveY() const { return activeY; }
    int getActiveRotation() const { return activeRotation; }
    int getGhostY() const { return ghostY(); }

    TetrominoType getHoldType() const;
    bool getHasHold() const;

    const std::vector<int> &getClearingRows() const;
    bool isClearing() const;

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
    Uint32 lockTimer;
    bool onGround;
    int getDropInterval() const;

    // Scoring
    int score;
    int lines;
    int level;

    GameState state;

    // Helpers
    void spawnPiece();
    void lockAndAdvance();

    TetrominoType holdType;
    bool hasHold;
    bool holdUsed; // prevents holding twice per piece

    std::vector<int> clearingRows;
    Uint32 clearTimer;
    bool clearing;
};