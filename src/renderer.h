#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool init();
    void render(const Game &game);

private:
    static constexpr int CELL = 30;
    static constexpr int BOARD_X = 20;
    static constexpr int BOARD_Y = 20;
    static constexpr int PREVIEW_X = 340;
    static constexpr int PREVIEW_Y = 80;
    static constexpr int HOLD_X = 340;
    static constexpr int HOLD_Y = 360;
    static constexpr int WIN_W = 480;
    static constexpr int WIN_H = 640;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    TTF_Font *font = nullptr;

    void drawBoard(const Game &game);
    void drawCell(int row, int col, int colorIdx, Uint8 alpha = 255);
    void drawPreview(const Game &game);
    void drawPiece(TetrominoType type, int rot, int px, int py,
                   Uint8 alpha);
    void drawUI(const Game &game);
    void drawOverlay(const char *text);
    SDL_Color getColor(int idx) const;
    void drawHold(const Game &game);
};