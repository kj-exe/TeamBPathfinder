#include "GameController.h"

GameController::GameController(GameEngine* engine, PuzzleRepository* repository)
{
    this->engine = engine;
    this->repository = repository;
    this->currentPuzzleIndex = 0;
}

void GameController::startPuzzle(int index)
{
    currentPuzzleIndex = index;
    Puzzle puzzle = repository->getPuzzle(index);
    engine->loadFromPuzzle(puzzle);
}

void GameController::resetCurrentPuzzle()
{
    startPuzzle(currentPuzzleIndex);
}

int GameController::getCurrentPuzzleNumber() const
{
    return currentPuzzleIndex + 1;
}

int GameController::getPuzzleCount() const
{
    return repository->getCount();
}

MoveResult GameController::attemptMove(int row, int col, int value)
{
    if (engine->isFixed(row, col))
        return MoveResult::InvalidCell;

    engine->removeNumber(row, col);

    if (!engine->placeNumber(row, col, value))
        return MoveResult::InvalidNumber;

    if (engine->isBoardComplete())
    {
        if (engine->isSolutionCorrect())
            return MoveResult::PuzzleSolved;
        else
            return MoveResult::PuzzleIncorrect;
    }

    return MoveResult::Accepted;
}

MoveResult GameController::clearCell(int row, int col)
{
    if (engine->isFixed(row, col))
        return MoveResult::InvalidCell;

    engine->removeNumber(row, col);
    return MoveResult::Cleared;
}

int GameController::getValue(int row, int col)
{
    return engine->getValue(row, col);
}

bool GameController::isFixed(int row, int col)
{
    return engine->isFixed(row, col);
}