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
    int solution[8][8];

public:
    Puzzle();
    Puzzle(int number);

    void addClue(int row, int col, int value);
    void setSolution(int row, int col, int value);

    int getNumber() const;
    const std::vector<Clue>& getClues() const;
    int getSolutionValue(int row, int col) const;
};