#pragma once

#include <string>
#include <vector>
#include "Puzzle.h"

class PuzzleRepository
{
private:
	std::vector<Puzzle> puzzles;


public:
	PuzzleRepository(const std::string& puzzleFilePath);


	Puzzle getPuzzle(int index) const;
	int getCount() const;
};
