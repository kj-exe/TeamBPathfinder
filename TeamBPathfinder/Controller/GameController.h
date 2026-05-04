#pragma once

#include "../Engine/GameEngine.h"
#include "../Model/PuzzleRepository.h"
#include "../Model/GameSnapshot.h"
#include <vector>

enum class MoveResult
{
    Accepted,
    Cleared,
    InvalidNumber,
    InvalidCell,
    PuzzleSolved,
    PuzzleIncorrect,
    Incomplete
};

class GameController
{
private:
    GameEngine* engine;
    PuzzleRepository* repository;
    int currentPuzzleIndex;
    std::vector<std::vector<std::vector<int>>> savedBoards;
    std::vector<int> savedSeconds;
    std::vector<bool> solvedPuzzles;

    void saveCurrentBoardToMemory();
    void restoreBoardFromMemory(int puzzleIndex);

public:
    GameController(GameEngine* engine, PuzzleRepository* repository);

    void startPuzzle(int index);
    void resetCurrentPuzzle();
    void initializeFirstPuzzle();
    void solveCurrentPuzzle();

    int getCurrentPuzzleNumber() const;
    int getCurrentPuzzleIndex() const;
    int getPuzzleCount() const;

    MoveResult attemptMove(int row, int col, int value);
    MoveResult clearCell(int row, int col);
    MoveResult submitPuzzle();

    int getValue(int row, int col) const;
    bool isFixed(int row, int col) const;
    bool isCurrentPuzzleSolved() const;

    GameSnapshot getSnapshot();
    void loadFromSnapshot(const GameSnapshot& snapshot);

    void tick();
    void resetTimer();
    int getElapsedSeconds() const;
};