#include "renderer.h"
#include <array>
#include <cstdio>

// COlors (indexed 0-7, 0 = unused)

static const SDL_Color colors[] = {
    {0, 0, 0},      // 0 - empty
    {45, 220, 255}, // 1 - I (cyan)
    {255, 255, 50}, // 2 - O (yellow)
    {170, 60, 200}, // 3 - T (purple)
    {80, 220, 80},  // 4 - S (green)
    {240, 60, 60},  // 5 - Z (red)
    {60, 80, 220},  // 6 - J (blue)
    {240, 160, 40}, // 7 - L (orange)
};

SDL_Color Renderer::getColor(int idx) const
{
    if (idx < 0 || idx > 7)
        return {255, 255, 255};
    return colors[idx];
}

// Init / Destroy

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    if (font)
        TTF_CloseFont(font);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
}

bool Renderer::init()
{
    window = SDL_CreateWindow("Tetris",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WIN_W, WIN_H, 0);
    if (!window)
        return false;

    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED);
    if (!renderer)
        return false;

    font = TTF_OpenFont("assets/fonts/arial.ttf", 20);
    if (!font)
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20);
    // Try common fallback paths, if still null, text will be skipped

    return true;
}

// Main render call
void Renderer::render(const Game &game)
{
    SDL_SetRenderDrawColor(renderer, 20, 20, 35, 255);
    SDL_RenderClear(renderer);

    drawBoard(game);
    drawPreview(game);
    drawHold(game);
    drawUI(game);

    if (game.getState() == GameState::Gameover)
        drawOverlay("GAME OVER");
    else if (game.getState() == GameState::Paused)
        drawOverlay("PAUSED");

    SDL_RenderPresent(renderer);
}

// Board

void Renderer::drawBoard(const Game &game)
{
    const Board &board = game.getBoard();

    // Background rectangle
    SDL_Rect bg = {BOARD_X - 2, BOARD_Y - 2,
                   Board::COLS * CELL,
                   Board::ROWS * CELL + 4};
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderFillRect(renderer, &bg);

    // Subtle grid lines on empty cells
    SDL_SetRenderDrawColor(renderer, 35, 35, 50, 255);
    for (int r = 0; r < Board::VISIBLE_ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c)
        {
            SDL_Rect cell = {BOARD_X + c * CELL, BOARD_Y + r * CELL, CELL, CELL};
            SDL_RenderDrawRect(renderer, &cell);
        }

    // Locked cells (skip hidden rows)
    for (int r = Board::HIDDEN_ROWS; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c)
        {
            int v = board.get(r, c);
            if (v)
            {
                bool flashing = false;
                if (game.isClearing())
                {
                    for (int fr : game.getClearingRows())
                        if (fr == r)
                        {
                            flashing = true;
                            break;
                        }
                }

                if (flashing)
                {
                    Uint32 t = SDL_GetTicks();
                    Uint8 brightness = (t / 60) % 2 == 0 ? 255 : 180;
                    SDL_Rect rect = {BOARD_X + c * CELL, BOARD_Y + (r - Board::HIDDEN_ROWS) * CELL, CELL, CELL};
                    SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
                else
                {
                    drawCell(r - Board::HIDDEN_ROWS, c, v);
                }
            }
            else
            {
                drawCell(r - Board::HIDDEN_ROWS, c, 0);
            }
        }

    // Ghost piece
    drawPiece(game.getActiveType(), game.getActiveRotation(),
              game.getActiveX(), game.getGhostY(), 80);

    // Active piece
    drawPiece(game.getActiveType(), game.getActiveRotation(),
              game.getActiveX(), game.getActiveY(), 255);
}

void Renderer::drawCell(int row, int col, int colorIdx, Uint8 alpha)
{
    SDL_Color c = getColor(colorIdx);
    int x = BOARD_X + col * CELL;
    int y = BOARD_Y + row * CELL;
    int s = CELL;

    // Main fill
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, alpha);
    SDL_Rect rect = {x, y, s, s};
    SDL_RenderFillRect(renderer, &rect);

    // Inner highlight (top + left, 2px, brighter)
    SDL_SetRenderDrawColor(renderer,
                           std::min(255, c.r + 80),
                           std::min(255, c.g + 80),
                           std::min(255, c.b + 80), alpha);
    SDL_RenderDrawLine(renderer, x + 1, y + 1, x + s - 2, y + 1);
    SDL_RenderDrawLine(renderer, x + 1, y + 1, x + 1, y + s - 2);

    // Inner shadow (bottom + right, 2px, darker)
    SDL_SetRenderDrawColor(renderer,
                           std::max(0, c.r - 80),
                           std::max(0, c.g - 80),
                           std::max(0, c.b - 80), alpha);
    SDL_RenderDrawLine(renderer, x + 1, y + s - 2, x + s - 2, y + s - 2);
    SDL_RenderDrawLine(renderer, x + s - 2, y + 1, x + s - 2, y + s - 2);

    // Small shine square top-left corner
    SDL_SetRenderDrawColor(renderer,
                           std::min(255, c.r + 120),
                           std::min(255, c.g + 120),
                           std::min(255, c.b + 120), alpha);
    SDL_Rect shine = {x + 3, y + 3, 4, 4};
    SDL_RenderFillRect(renderer, &shine);

    // Outer dark border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_RenderDrawRect(renderer, &rect);
}

// Piece rendering

void Renderer::drawPiece(TetrominoType type, int rot,
                         int px, int py, Uint8 alpha)
{
    static const std::array<Tetromino, 7> pieces = createTetrominoes();
    const auto &shape = pieces[static_cast<int>(type)].rotations[rot];
    int colorIdx = pieces[static_cast<int>(type)].color;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (shape[r][c])
            {
                int row = py + r - Board::HIDDEN_ROWS;
                int col = px + c;
                if (row >= 0 && row < Board::VISIBLE_ROWS)
                    drawCell(row, col, colorIdx, alpha);
            }
}

// Preview

void Renderer::drawPreview(const Game &game)
{
    // "NEXT" label
    if (font)
    {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *surf = TTF_RenderText_Blended(font, "NEXT", white);
        if (surf)
        {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {PREVIEW_X, PREVIEW_Y - 30, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }

    // Draw next piece centered in the preview box
    static const std::array<Tetromino, 7> pieces = createTetrominoes();
    TetrominoType next = game.getNextPiece();
    int colorIdx = pieces[static_cast<int>(next)].color;
    const auto &shape = pieces[static_cast<int>(next)].rotations[0];

    int minC = 4, maxC = 0, minR = 4, maxR = 0;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (shape[r][c])
            {
                if (c < minC)
                    minC = c;
                if (c > maxC)
                    maxC = c;
                if (r < minR)
                    minR = r;
                if (r > maxR)
                    maxR = r;
            }
    int pW = (maxC - minC + 1) * CELL;
    int pH = (maxR - minR + 1) * CELL;
    int offX = PREVIEW_X + (4 * CELL - pW) / 2 - minC * CELL;
    int offY = PREVIEW_Y + (4 * CELL - pH) / 2 - minR * CELL;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (shape[r][c])
            {
                SDL_Rect rect = {offX + c * CELL, offY + r * CELL,
                                 CELL, CELL};
                SDL_Color col = getColor(colorIdx);
                SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, col.r / 2, col.g / 2, col.b / 2, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
}

void Renderer::drawHold(const Game &game)
{
    // Dark background box (always visible even when empty)
    SDL_Rect box = {HOLD_X - 5, HOLD_Y - 35, 4 * CELL + 10, 4 * CELL + 45};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &box);

    if (!game.getHasHold())
        return;

    // "Hold" label
    if (font)
    {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *surf = TTF_RenderText_Blended(font, "HOLD", white);
        if (surf)
        {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {HOLD_X, HOLD_Y - 30, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }

    static const std::array<Tetromino, 7> pieces = createTetrominoes();
    TetrominoType hold = game.getHoldType();
    int colorIdx = pieces[static_cast<int>(hold)].color;
    const auto &shape = pieces[static_cast<int>(hold)].rotations[0];

    int minC = 4, maxC = 0, minR = 4, maxR = 0;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (shape[r][c])
            {
                if (c < minC)
                    minC = c;
                if (c > maxC)
                    maxC = c;
                if (r < minR)
                    minR = r;
                if (r > maxR)
                    maxR = r;
            }
    int pW = (maxC - minC + 1) * CELL;
    int pH = (maxR - minR + 1) * CELL;
    int offX = HOLD_X + (4 * CELL - pW) / 2 - minC * CELL;
    int offY = HOLD_Y + (4 * CELL - pH) / 2 - minR * CELL;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (shape[r][c])
            {
                SDL_Rect rect = {offX + c * CELL, offY + r * CELL, CELL, CELL};
                SDL_Color col = getColor(colorIdx);
                SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, col.r / 2, col.g / 2, col.b / 2, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
}

// Sidebar UI

void Renderer::drawUI(const Game &game)
{
    if (!font)
        return;

    SDL_Color white = {255, 255, 255, 255};
    char buf[64];

    auto renderText = [&](const char *label, int value, int y)
    {
        std::snprintf(buf, sizeof(buf), "%s: %d", label, value);
        SDL_Surface *surf = TTF_RenderText_Blended(font, buf, white);
        if (!surf)
            return;
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = {PREVIEW_X, y, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    };

    renderText("SCORE", game.getScore(), 220);
    renderText("LINES", game.getLine(), 250);
    renderText("LEVEL", game.getLevel(), 280);
}

// Overlay (pause / game over)

void Renderer::drawOverlay(const char *text)
{
    if (!font)
        return;

    // Dim background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_Rect full = {0, 0, WIN_W, WIN_H};
    SDL_RenderFillRect(renderer, &full);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, white);
    if (!surf)
        return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect dst = {(WIN_W - surf->w) / 2,
                    (WIN_H - surf->h) / 2,
                    surf->w, surf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}