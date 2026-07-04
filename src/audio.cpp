#include "audio.h"
#include <SDL2/SDL.h>
#include <iostream>

Audio::Audio() {}

Audio::~Audio()
{
    if (sMove)
        Mix_FreeChunk(sMove);
    if (sRotate)
        Mix_FreeChunk(sRotate);
    if (sLock)
        Mix_FreeChunk(sLock);
    if (sClear)
        Mix_FreeChunk(sClear);
    if (sTetris)
        Mix_FreeChunk(sTetris);
    if (sGameOver)
        Mix_FreeChunk(sGameOver);
    if (sOpenMenu)
        Mix_FreeChunk(sOpenMenu);
    if (sMoveMenu)
        Mix_FreeChunk(sMoveMenu);
    if (sClickMenu)
        Mix_FreeChunk(sClickMenu);
    if (sExitMenu)
        Mix_FreeChunk(sExitMenu);
    if (music)
        Mix_FreeMusic(music);
    Mix_CloseAudio();
}

bool Audio::init()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        return false;
    }

    music = Mix_LoadMUS("assets/sounds/Tetris Theme.mp3");
    sMove = Mix_LoadWAV("assets/sounds/move.wav");
    sRotate = Mix_LoadWAV("assets/sounds/rotate.wav");
    sLock = Mix_LoadWAV("assets/sounds/lock.wav");
    sClear = Mix_LoadWAV("assets/sounds/clear line.wav");
    sTetris = Mix_LoadWAV("assets/sounds/Tetris 4 level.wav");
    sGameOver = Mix_LoadWAV("assets/sounds/Game Over.wav");
    sOpenMenu = Mix_LoadWAV("assets/sounds/open menu.wav");
    sClickMenu = Mix_LoadWAV("assets/sounds/click menu.wav");
    sMoveMenu = Mix_LoadWAV("assets/sounds/move menu.wav");
    sExitMenu = Mix_LoadWAV("assets/sounds/exit menu.mp3");

    return true;
}

void Audio::playMusic()
{
    if (music)
        Mix_PlayMusic(music, -1); // -1 = loop forever
}

void Audio::pauseMusic()
{
    Mix_PauseMusic();
}

void Audio::resumeMusic()
{
    Mix_ResumeMusic();
}

void Audio::playMove()
{
    if (sMove)
        Mix_PlayChannel(-1, sMove, 0);
}
void Audio::playRotate()
{
    if (sRotate)
        Mix_PlayChannel(-1, sRotate, 0);
}
void Audio::playLock()
{
    if (sLock)
        Mix_PlayChannel(-1, sLock, 0);
}
void Audio::playClear()
{
    if (sClear)
        Mix_PlayChannel(-1, sClear, 0);
}
void Audio::playTetris()
{
    if (sTetris)
        Mix_PlayChannel(-1, sTetris, 0);
}
void Audio::playGameOver()
{
    if (sGameOver)
        Mix_PlayChannel(-1, sGameOver, 0);
}
void Audio::playOpenMenu()
{
    if (sOpenMenu)
        Mix_PlayChannel(-1, sOpenMenu, 0);
}
void Audio::playMoveMenu()
{
    if (sMoveMenu)
        Mix_PlayChannel(-1, sMoveMenu, 0);
}
void Audio::playClickMenu()
{
    if (sClickMenu)
        Mix_PlayChannel(-1, sClickMenu, 0);
}
void Audio::playExitMenu()
{
    if (sExitMenu)
        Mix_PlayChannel(-1, sExitMenu, 0); // play once
}