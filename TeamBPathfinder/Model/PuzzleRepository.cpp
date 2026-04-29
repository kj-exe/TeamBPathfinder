#include "PuzzleRepository.h"

PuzzleRepository::PuzzleRepository()
{
    buildPuzzleA();
	buildPuzzleB();
}

void PuzzleRepository::buildPuzzleA()
{
    Puzzle puzzle(1);

    int solution[8][8] = {
        {  1,  2,  3,  4,  5,  6,  7,  8 },
        { 28, 29, 54, 53, 46, 45, 44,  9 },
        { 27, 30, 55, 52, 47, 48, 43, 10 },
        { 26, 31, 56, 51, 50, 49, 42, 11 },
        { 25, 32, 57, 64, 63, 62, 41, 12 },
        { 24, 33, 58, 59, 60, 61, 40, 13 },
        { 23, 34, 35, 36, 37, 38, 39, 14 },
        { 22, 21, 20, 19, 18, 17, 16, 15 }
    };

    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            puzzle.setSolution(row, col, solution[row][col]);

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

void PuzzleRepository::buildPuzzleB()
{
    Puzzle puzzle(2);

    int solution[8][8] = {
        { 42, 41, 40, 39, 32, 31, 26, 25 },
        { 43, 48, 49, 38, 33, 30, 27, 24 },
        { 44, 47, 50, 37, 34, 29, 28, 23 },
        { 45, 46, 51, 36, 35, 20, 21, 22 },
        { 54, 53, 52,  1,  2, 19, 16, 15 },
        { 55, 60, 61,  4,  3, 18, 17, 14 },
        { 56, 59, 62,  5,  6,  9, 10, 13 },
        { 57, 58, 63, 64,  7,  8, 11, 12 }
    };

    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            puzzle.setSolution(row, col, solution[row][col]);

    puzzle.addClue(0, 0, 42);
    puzzle.addClue(0, 3, 39);
    puzzle.addClue(0, 4, 32);
    puzzle.addClue(0, 7, 25);
    puzzle.addClue(3, 0, 45);
    puzzle.addClue(3, 2, 51);
    puzzle.addClue(3, 3, 36);
    puzzle.addClue(3, 4, 35);
    puzzle.addClue(3, 5, 20);
    puzzle.addClue(3, 7, 22);
    puzzle.addClue(4, 1, 53);
    puzzle.addClue(4, 3, 1);
    puzzle.addClue(4, 4, 2);
    puzzle.addClue(4, 6, 16);
    puzzle.addClue(6, 3, 5);
    puzzle.addClue(6, 4, 6);
    puzzle.addClue(7, 0, 57);
    puzzle.addClue(7, 3, 64);
    puzzle.addClue(7, 4, 7);
    puzzle.addClue(7, 7, 12);

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