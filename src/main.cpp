#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "game.h"
#include "renderer.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Renderer renderer;
    if (!renderer.init())
    {
        std::cerr << "Renderer init failed" << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Game game;

    static const Uint32 DAS_DELAY = 150;
    static const Uint32 DAS_REPEAT = 50;

    bool leftHeld = false, rightHeld = false, downHeld = false;
    Uint32 leftTimer = 0, rightTimer = 0, downTimer = 0;
    bool leftRepeating = false, rightRepeating = false, downRepeating = false;

    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();

    while (running)
    {
        Uint32 currentTicks = SDL_GetTicks();
        Uint32 dt = currentTicks - lastTicks;
        lastTicks = currentTicks;

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN && !e.key.repeat)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_a:
                case SDLK_LEFT:
                    game.moveLeft();
                    leftHeld = true;
                    leftTimer = 0;
                    leftRepeating = false;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    game.moveRight();
                    rightHeld = true;
                    rightTimer = 0;
                    rightRepeating = false;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    game.softDrop();
                    downHeld = true;
                    downTimer = 0;
                    downRepeating = false;
                    break;
                case SDLK_w:
                case SDLK_UP:
                    game.rotateCW();
                    break;
                case SDLK_SPACE:
                    game.hardDrop();
                    break;
                case SDLK_p:
                    game.togglePause();
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    game.holdPiece();
                    break;
                case SDLK_r:
                    game.rotateCCW();
                    break;
                }
            }
            else if (e.type == SDL_KEYUP)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_a:
                case SDLK_LEFT:
                    leftHeld = false;
                    leftRepeating = false;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    rightHeld = false;
                    rightRepeating = false;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    downHeld = false;
                    downRepeating = false;
                    break;
                }
            }
        }

        if (leftHeld)
        {
            leftTimer += dt;
            if (leftTimer >= (leftRepeating ? DAS_REPEAT : DAS_DELAY))
            {
                leftTimer = 0;
                leftRepeating = true;
                game.moveLeft();
            }
        }
        if (rightHeld)
        {
            rightTimer += dt;
            if (rightTimer >= (rightRepeating ? DAS_REPEAT : DAS_DELAY))
            {
                rightTimer = 0;
                rightRepeating = true;
                game.moveRight();
            }
        }
        if (downHeld)
        {
            downTimer += dt;
            if (downTimer >= (downRepeating ? 30 : DAS_DELAY))
            {
                downTimer = 0;
                downRepeating = true;
                game.softDrop();
            }
        }

        game.update(dt);

        game.update(dt);
        renderer.render(game);

        SDL_Delay(16); // ~60 FPS cap
    }

    TTF_Quit();
    SDL_Quit();
    return 0;
}