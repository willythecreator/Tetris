#pragma once
#include <SDL2/SDL_mixer.h>

class Audio
{
public:
    Audio();
    ~Audio();

    bool init();

    // Music
    void playMusic();
    void pauseMusic();
    void resumeMusic();

    // Sound effects
    void playMove();
    void playRotate();
    void playLock();
    void playClear();
    void playTetris();
    void playGameOver();
    void playOpenMenu();
    void playMoveMenu();
    void playClickMenu();
    void playExitMenu();

private:
    Mix_Music *music = nullptr;
    Mix_Chunk *sMove = nullptr;
    Mix_Chunk *sRotate = nullptr;
    Mix_Chunk *sLock = nullptr;
    Mix_Chunk *sClear = nullptr;
    Mix_Chunk *sTetris = nullptr;
    Mix_Chunk *sGameOver = nullptr;
    Mix_Chunk *sOpenMenu = nullptr;
    Mix_Chunk *sMoveMenu = nullptr;
    Mix_Chunk *sClickMenu = nullptr;
    Mix_Chunk *sExitMenu = nullptr;
};