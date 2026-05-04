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

    this->savedSeconds.resize(puzzleCount, 0);
    this->solvedPuzzles.resize(puzzleCount, false);
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
    this->savedSeconds[this->currentPuzzleIndex] = 0;
    this->solvedPuzzles[this->currentPuzzleIndex] = false;

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

void GameController::initializeFirstPuzzle()
{
    this->currentPuzzleIndex = 0;

    Puzzle puzzle = this->repository->getPuzzle(0);
    this->engine->loadFromPuzzle(puzzle);

    this->restoreBoardFromMemory(0);
}

void GameController::solveCurrentPuzzle()
{
    Puzzle puzzle = this->repository->getPuzzle(this->currentPuzzleIndex);

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            if (!this->engine->isFixed(row, col))
            {
                this->engine->removeNumber(row, col);
                this->engine->placeNumber(row, col, puzzle.getSolutionValue(row, col));
            }
        }
    }

    this->saveCurrentBoardToMemory();
}

int GameController::getCurrentPuzzleNumber() const
{
    return this->currentPuzzleIndex + 1;
}

int GameController::getCurrentPuzzleIndex() const
{
    return this->currentPuzzleIndex;
}

int GameController::getPuzzleCount() const
{
    return this->repository->getCount();
}

MoveResult GameController::attemptMove(int row, int col, int value)
{
    if (this->engine->isFixed(row, col))
    {
        return MoveResult::InvalidCell;
    }

    this->engine->removeNumber(row, col);

    if (!this->engine->placeNumber(row, col, value))
    {
        return MoveResult::InvalidNumber;
    }

    return MoveResult::Accepted;
}

MoveResult GameController::clearCell(int row, int col)
{
    if (this->engine->isFixed(row, col))
    {
        return MoveResult::InvalidCell;
    }

    this->engine->removeNumber(row, col);
    return MoveResult::Cleared;
}

MoveResult GameController::submitPuzzle()
{
    if (!this->engine->isBoardComplete())
    {
        return MoveResult::Incomplete;
    }

    if (this->engine->isSolutionCorrect())
    {
        this->solvedPuzzles[this->currentPuzzleIndex] = true;
        this->saveCurrentBoardToMemory();
        return MoveResult::PuzzleSolved;
    }

    return MoveResult::PuzzleIncorrect;
}

int GameController::getValue(int row, int col) const
{
    return this->engine->getValue(row, col);
}

bool GameController::isFixed(int row, int col) const
{
    return this->engine->isFixed(row, col);
}

bool GameController::isCurrentPuzzleSolved() const
{
    return this->solvedPuzzles[this->currentPuzzleIndex];
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
            int value = this->savedBoards[puzzleIndex][row][col];

            if (!this->engine->isFixed(row, col))
            {
                this->engine->setEditableValue(row, col, value);
            }
        }
    }
}

GameSnapshot GameController::getSnapshot()
{
    this->saveCurrentBoardToMemory();

    int puzzleCount = (int)this->savedBoards.size();
    GameSnapshot snapshot(puzzleCount);

    snapshot.setCurrentPuzzleIndex(this->currentPuzzleIndex);

    for (int puzzleIndex = 0; puzzleIndex < puzzleCount; puzzleIndex++)
    {
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                snapshot.setValue(
                    puzzleIndex,
                    row,
                    col,
                    this->savedBoards[puzzleIndex][row][col]
                );
            }
        }

        snapshot.setSolved(puzzleIndex, this->solvedPuzzles[puzzleIndex]);
        snapshot.setSeconds(puzzleIndex, this->savedSeconds[puzzleIndex]);
    }

    return snapshot;
}

void GameController::loadFromSnapshot(const GameSnapshot& snapshot)
{
    int puzzleCount = (int)this->savedBoards.size();

    if (snapshot.getBoardCount() != puzzleCount)
    {
        return;
    }

    for (int puzzleIndex = 0; puzzleIndex < puzzleCount; puzzleIndex++)
    {
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                this->savedBoards[puzzleIndex][row][col] =
                    snapshot.getValue(puzzleIndex, row, col);
            }
        }

        this->solvedPuzzles[puzzleIndex] = snapshot.getSolved(puzzleIndex);
        this->savedSeconds[puzzleIndex] = snapshot.getSeconds(puzzleIndex);
    }

    int targetIndex = snapshot.getCurrentPuzzleIndex();

    if (targetIndex < 0 || targetIndex >= puzzleCount)
    {
        return;
    }

    this->currentPuzzleIndex = targetIndex;

    Puzzle puzzle = this->repository->getPuzzle(targetIndex);
    this->engine->loadFromPuzzle(puzzle);

    this->restoreBoardFromMemory(targetIndex);
}

void GameController::tick()
{
    if (!this->solvedPuzzles[this->currentPuzzleIndex])
    {
        this->savedSeconds[this->currentPuzzleIndex]++;
    }
}

void GameController::resetTimer()
{
    this->savedSeconds[this->currentPuzzleIndex] = 0;
}

int GameController::getElapsedSeconds() const
{
    return this->savedSeconds[this->currentPuzzleIndex];
}