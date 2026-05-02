#pragma once

#include "../Engine/GameEngine.h"
#include "../Model/PuzzleRepository.h"
#include <vector>

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
    std::vector<std::vector<std::vector<int>>> savedBoards;

public:
    GameController(GameEngine* engine, PuzzleRepository* repository);
    void solveCurrentPuzzle();

    void startPuzzle(int index);
    void resetCurrentPuzzle();
    int getCurrentPuzzleNumber() const;
    int getPuzzleCount() const;

    MoveResult attemptMove(int row, int col, int value);
    MoveResult clearCell(int row, int col);
    MoveResult submitPuzzle();

    int getValue(int row, int col) const;
    bool isFixed(int row, int col) const;

    int getCurrentPuzzleIndex() const;
    void restoreEditableValue(int row, int col, int value);

    void saveCurrentBoardToMemory();
    void restoreBoardFromMemory(int puzzleIndex);

    int getSavedBoardValue(int puzzleIndex, int row, int col) const;
    void setSavedBoardValue(int puzzleIndex, int row, int col, int value);
    int getSavedBoardCount() const;

    void loadPuzzleFromMemory(int index);

    void initializeFirstPuzzle();
};