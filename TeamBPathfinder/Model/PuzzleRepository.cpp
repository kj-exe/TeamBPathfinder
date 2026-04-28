#include "PuzzleRepository.h"

PuzzleRepository::PuzzleRepository()
{
    buildPuzzleA();
}

void PuzzleRepository::buildPuzzleA()
{
    Puzzle puzzle(1);

    puzzle.addClue(0, 0, 1);
    puzzle.addClue(0, 7, 8);
    puzzle.addClue(1, 1, 29);
    puzzle.addClue(1, 6, 44);
    puzzle.addClue(3, 2, 56);
    puzzle.addClue(3, 5, 49);
    puzzle.addClue(5, 2, 58);
    puzzle.addClue(5, 5, 61);
    puzzle.addClue(6, 1, 34);
    puzzle.addClue(6, 6, 39);
    puzzle.addClue(7, 0, 22);
    puzzle.addClue(7, 7, 15);

    puzzles.push_back(puzzle);
}

Puzzle PuzzleRepository::getPuzzle(int index) const
{
    return puzzles[index];
}

int PuzzleRepository::getCount() const
{
    return (int)puzzles.size();
}