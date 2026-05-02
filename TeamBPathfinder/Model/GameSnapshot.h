#pragma once

#include <vector>

class GameSnapshot
{
private:
    int currentPuzzleIndex;
    std::vector<std::vector<std::vector<int>>> boards;

public:
    GameSnapshot();
    GameSnapshot(int puzzleCount);

    int getCurrentPuzzleIndex() const;
    void setCurrentPuzzleIndex(int index);

    int getBoardCount() const;
    int getValue(int puzzleIndex, int row, int col) const;
    void setValue(int puzzleIndex, int row, int col, int value);
};