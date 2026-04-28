#pragma once

#include "../Engine/GameEngine.h"
#include "../Model/PuzzleRepository.h"

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
    MoveResult submitPuzzle();

    int getValue(int row, int col);
    bool isFixed(int row, int col);
};