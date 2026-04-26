#include "Puzzle.h"

Puzzle::Puzzle()
{
    this->number = 0;
}

Puzzle::Puzzle(int number)
{
    this->number = number;
}

void Puzzle::addClue(int row, int col, int value)
{
    Clue clue;
    clue.row = row;
    clue.col = col;
    clue.value = value;
    clues.push_back(clue);
}

int Puzzle::getNumber() const
{
    return number;
}

const std::vector<Clue>& Puzzle::getClues() const
{
    return clues;
}