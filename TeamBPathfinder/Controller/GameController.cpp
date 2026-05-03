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
}

void GameController::startPuzzle(int index)
{
    saveCurrentBoardToMemory();
    this->currentPuzzleIndex = index;

    Puzzle puzzle = this->repository->getPuzzle(index);
    this->engine->loadFromPuzzle(puzzle);

    restoreBoardFromMemory(index);
}

void GameController::resetCurrentPuzzle()
{
    this->savedSeconds[this->currentPuzzleIndex] = 0;

    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            this->savedBoards[this->currentPuzzleIndex][row][col] = 0;

    Puzzle puzzle = this->repository->getPuzzle(this->currentPuzzleIndex);
    this->engine->loadFromPuzzle(puzzle);
}

void GameController::initializeFirstPuzzle()
{
    this->currentPuzzleIndex = 0;
    Puzzle puzzle = this->repository->getPuzzle(0);
    this->engine->loadFromPuzzle(puzzle);
    restoreBoardFromMemory(0);
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

int GameController::getCurrentPuzzleNumber() const
{
    return currentPuzzleIndex + 1;
}

int GameController::getCurrentPuzzleIndex() const
{
    return currentPuzzleIndex;
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

MoveResult GameController::submitPuzzle()
{
    if (!engine->isBoardComplete())
        return MoveResult::Incomplete;
    if (engine->isSolutionCorrect())
        return MoveResult::PuzzleSolved;
    return MoveResult::PuzzleIncorrect;
}

int GameController::getValue(int row, int col) const
{
    return engine->getValue(row, col);
}

bool GameController::isFixed(int row, int col) const
{
    return engine->isFixed(row, col);
}

void GameController::saveCurrentBoardToMemory()
{
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            this->savedBoards[this->currentPuzzleIndex][row][col] =
            this->engine->getValue(row, col);
}

void GameController::restoreBoardFromMemory(int puzzleIndex)
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            int value = this->savedBoards[puzzleIndex][row][col];
            if (!engine->isFixed(row, col))
                engine->setEditableValue(row, col, value);
        }
    }
}

GameSnapshot GameController::getSnapshot()
{
    saveCurrentBoardToMemory();

    int puzzleCount = (int)this->savedBoards.size();
    GameSnapshot snapshot(puzzleCount);
    snapshot.setCurrentPuzzleIndex(currentPuzzleIndex);

    for (int puzzleIndex = 0; puzzleIndex < puzzleCount; puzzleIndex++)
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                snapshot.setValue(puzzleIndex, row, col, savedBoards[puzzleIndex][row][col]);

    return snapshot;
}

void GameController::loadFromSnapshot(const GameSnapshot& snapshot)
{
    int puzzleCount = (int)this->savedBoards.size();

    if (snapshot.getBoardCount() != puzzleCount)
        return;

    for (int puzzleIndex = 0; puzzleIndex < puzzleCount; puzzleIndex++)
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++)
                savedBoards[puzzleIndex][row][col] = snapshot.getValue(puzzleIndex, row, col);

    int targetIndex = snapshot.getCurrentPuzzleIndex();
    if (targetIndex < 0 || targetIndex >= puzzleCount)
        return;

    this->currentPuzzleIndex = targetIndex;
    Puzzle puzzle = this->repository->getPuzzle(targetIndex);
    this->engine->loadFromPuzzle(puzzle);
    restoreBoardFromMemory(targetIndex);
}

void GameController::tick()
{
    savedSeconds[currentPuzzleIndex]++;
}

void GameController::resetTimer()
{
    savedSeconds[currentPuzzleIndex] = 0;
}

int GameController::getElapsedSeconds() const
{
    return savedSeconds[currentPuzzleIndex];
}