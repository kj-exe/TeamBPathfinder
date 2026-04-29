#include "Puzzle.h"

Puzzle::Puzzle()
{
    this->number = 0;
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            solution[row][col] = 0;
}

Puzzle::Puzzle(int number)
{
    this->number = number;
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            solution[row][col] = 0;
}

void Puzzle::addClue(int row, int col, int value)
{
    Clue clue;
    clue.row = row;
    clue.col = col;
    clue.value = value;
    clues.push_back(clue);
}

void Puzzle::setSolution(int row, int col, int value)
{
    solution[row][col] = value;
}

int Puzzle::getNumber() const
{
    return number;
}

const std::vector<Clue>& Puzzle::getClues() const
{
    return clues;
}

int Puzzle::getSolutionValue(int row, int col) const
{
    return solution[row][col];
}