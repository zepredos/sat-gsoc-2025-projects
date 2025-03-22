# 15 Puzzle Game

## Overview
The 15 Puzzle Game is a classic sliding puzzle implemented in C++. The game consists of a 4x4 board containing 15 numbered tiles and one empty space. The objective is to arrange the tiles in ascending order by sliding them into the empty space.

## Features
- 4x4 sliding puzzle with 15 numbered tiles.
- Randomised starting configuration using a custom seeded random generator.
- Interactive gameplay via keyboard input.
- Win detection when the board is solved.

## Build Instructions
Ensure that `Random.h` is in the same directory as `15puzzlegame.cpp`. Then compile with:
```sh
g++ -std=c++17 15puzzlegame.cpp Random.h -o 15puzzlegame
```
## Usage
1. Run the game:
```sh
./15puzzlegame
```
2. The board is displayed on the console.

3. Use the following keys to play:

`w`, `a`, `s`, `d` to move the empty tile.

4. `q` to quit the game.

5. The game updates the board after each move and notifies you when you win.
