#pragma once

#include <vector>
#include "Puzzle.h"

class PuzzleRepository
{
private:
    std::vector<Puzzle> puzzles;

    void buildPuzzleA();
    void buildPuzzleB();

public:
    PuzzleRepository();

    Puzzle getPuzzle(int index) const;
    int getCount() const;
};