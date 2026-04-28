#pragma once

#include <vector>

struct Clue
{
    int row;
    int col;
    int value;
};

class Puzzle
{
private:
    int number;
    std::vector<Clue> clues;

public:
    Puzzle();
    Puzzle(int number);

    void addClue(int row, int col, int value);

    int getNumber() const;
    const std::vector<Clue>& getClues() const;
};