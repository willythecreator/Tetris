# Tetris

A clean, fast Tetris clone written in **C++17** with **SDL2** — built from scratch with custom block rendering, ghost piece, and level-based gravity.

![Status](https://img.shields.io/badge/status-active%20development-brightgreen)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)
![License](https://img.shields.io/badge/license-MIT-purple)

---

## Gameplay

| Key | Action |
|-----|--------|
| `←` `→` | Move piece |
| `↑` | Rotate clockwise |
| `↓` | Soft drop |
| `Space` | Hard drop |
| `P` | Pause / Resume |
| `Esc` | Quit |

---

## Features

- **7-bag randomizer** — fair piece distribution, no droughts
- **Ghost piece** — see exactly where your piece will land
- **Level system** — gravity speeds up every 10 lines cleared
- **Scoring** — combo multipliers for clearing multiple lines at once
- **Bevel-shaded blocks** — custom SDL2 block renderer with highlight and shadow
- **Next piece preview**
- **Pause & Game Over overlays**

## Building

### Requirements

- [MSYS2](https://www.msys2.org/) with MinGW-w64
- CMake 3.16+
- SDL2, SDL2_ttf

### Install dependencies (MSYS2 MINGW64 terminal)

```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

### Font setup

Place any `.ttf` font file at:

```
assets/fonts/font.ttf
```

You can copy one from your system:

```
copy C:\Windows\Fonts\segoeui.ttf assets\fonts\font.ttf
```

### Build (cmd.exe)

```
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
mingw32-make
```

### Run

```
copy C:\msys64\mingw64\bin\SDL2.dll .
copy C:\msys64\mingw64\bin\SDL2_ttf.dll .
Tetris.exe
```

---

## Project Structure

```
Tetris/
├── src/
│   ├── main.cpp              # SDL event loop
│   ├── game.cpp/.h           # Game logic, gravity, scoring
│   ├── board.cpp/.h          # Grid state, collision, line clears
│   ├── tetromino.cpp/.h      # Piece definitions & rotations
│   ├── renderer.cpp/.h       # SDL2 rendering
│   ├── audio.cpp/.h          # (coming soon)
│   └── scoremanager.cpp/.h   # (coming soon)
├── assets/
│   └── fonts/                # Place your font.ttf here (not committed)
└── CMakeLists.txt
```

---