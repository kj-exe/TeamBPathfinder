#include "GameSnapshot.h"

GameSnapshot::GameSnapshot()
{
    this->currentPuzzleIndex = 0;
}

GameSnapshot::GameSnapshot(int puzzleCount)
{
    this->currentPuzzleIndex = 0;
    this->boards.resize(
        puzzleCount,
        std::vector<std::vector<int>>(
            8,
            std::vector<int>(8, 0)
        )
    );
}

int GameSnapshot::getCurrentPuzzleIndex() const
{
    return this->currentPuzzleIndex;
}

void GameSnapshot::setCurrentPuzzleIndex(int index)
{
    this->currentPuzzleIndex = index;
}

int GameSnapshot::getBoardCount() const
{
    return (int)this->boards.size();
}

int GameSnapshot::getValue(int puzzleIndex, int row, int col) const
{
    return this->boards[puzzleIndex][row][col];
}

void GameSnapshot::setValue(int puzzleIndex, int row, int col, int value)
{
    this->boards[puzzleIndex][row][col] = value;
}