#include "GameController.h"

GameController::GameController(GameEngine* engine, PuzzleRepository* repository)
{
    this->engine = engine;
    this->repository = repository;
    this->currentPuzzleIndex = 0;

    int puzzleCount = repository->getCount();

    this->savedBoards.resize(
        puzzleCount,
        std::vector<std::vector<int>>(
            8,
            std::vector<int>(8, 0)
        )
    );
}

void GameController::solveCurrentPuzzle()
{
    Puzzle puzzle = repository->getPuzzle(currentPuzzleIndex);
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            if (!engine->isFixed(row, col))
            {
                engine->removeNumber(row, col);
                engine->placeNumber(row, col, puzzle.getSolutionValue(row, col));
            }
        }
    }
}

void GameController::startPuzzle(int index)
{
    this->saveCurrentBoardToMemory();

    this->currentPuzzleIndex = index;

    Puzzle puzzle = this->repository->getPuzzle(index);
    this->engine->loadFromPuzzle(puzzle);

	this->restoreBoardFromMemory(index);
}

void GameController::resetCurrentPuzzle()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            this->savedBoards[this->currentPuzzleIndex][row][col] = 0;
        }
    }

    Puzzle puzzle = this->repository->getPuzzle(this->currentPuzzleIndex);
    this->engine->loadFromPuzzle(puzzle);
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

    return MoveResult::Accepted;
}

MoveResult GameController::clearCell(int row, int col)
{
    if (engine->isFixed(row, col))
        return MoveResult::InvalidCell;

    engine->removeNumber(row, col);
    return MoveResult::Cleared;
}

int GameController::getValue(int row, int col) const
{
    return engine->getValue(row, col);
}

bool GameController::isFixed(int row, int col) const
{
    return engine->isFixed(row, col);
}

MoveResult GameController::submitPuzzle()
{
    if (!engine->isBoardComplete())
        return MoveResult::InvalidNumber;
    if (engine->isSolutionCorrect())
        return MoveResult::PuzzleSolved;
    return MoveResult::PuzzleIncorrect;
}

int GameController::getCurrentPuzzleIndex() const
{
    return currentPuzzleIndex;
}

void GameController::restoreEditableValue(int row, int col, int value)
{
    if (!engine->isFixed(row, col))
    {
        engine->setEditableValue(row, col, value);
    }
}

void GameController::saveCurrentBoardToMemory()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            this->savedBoards[this->currentPuzzleIndex][row][col] =
                this->engine->getValue(row, col);
        }
    }
}

void GameController::restoreBoardFromMemory(int puzzleIndex)
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            this->restoreEditableValue(
                row,
                col,
                this->savedBoards[puzzleIndex][row][col]
            );
        }
    }
}

int GameController::getSavedBoardValue(int puzzleIndex, int row, int col) const
{
    return this->savedBoards[puzzleIndex][row][col];
}

void GameController::setSavedBoardValue(int puzzleIndex, int row, int col, int value)
{
    this->savedBoards[puzzleIndex][row][col] = value;
}

int GameController::getSavedBoardCount() const
{
    return (int)this->savedBoards.size();
}

void GameController::loadPuzzleFromMemory(int index)
{
    this->currentPuzzleIndex = index;

    Puzzle puzzle = this->repository->getPuzzle(index);
    this->engine->loadFromPuzzle(puzzle);

    this->restoreBoardFromMemory(index);
}

void GameController::initializeFirstPuzzle()
{
    this->currentPuzzleIndex = 0;

    Puzzle puzzle = this->repository->getPuzzle(0);
    this->engine->loadFromPuzzle(puzzle);

    this->restoreBoardFromMemory(0);
}