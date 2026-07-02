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

    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_LEFT:
                    game.moveLeft();
                    break;
                case SDLK_RIGHT:
                    game.moveRight();
                    break;
                case SDLK_DOWN:
                    game.softDrop();
                    break;
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
                }
            }
        }

        Uint32 currentTicks = SDL_GetTicks();
        Uint32 dt = currentTicks - lastTicks;
        lastTicks = currentTicks;

        game.update(dt);
        renderer.render(game);

        SDL_Delay(16); // ~60 FPS cap
    }

    TTF_Quit();
    SDL_Quit();
    return 0;
}