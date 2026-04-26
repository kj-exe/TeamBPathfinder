#pragma once

#include "GameEngine.h"
#include "PuzzleRepository.h"

enum class MoveResult
{
    Accepted,
    Cleared,
    InvalidNumber,
    InvalidCell,
    PuzzleSolved,
    PuzzleIncorrect
};

class GameController
{
private:
    GameEngine* engine;
    PuzzleRepository* repository;
    int currentPuzzleIndex;

public:
    GameController(GameEngine* engine, PuzzleRepository* repository);

    void startPuzzle(int index);
    void resetCurrentPuzzle();
    int getCurrentPuzzleNumber() const;
    int getPuzzleCount() const;

    MoveResult attemptMove(int row, int col, int value);
    MoveResult clearCell(int row, int col);

    int getValue(int row, int col);
    bool isFixed(int row, int col);
};