#pragma once

#include <string>
#include <vector>
#include "Puzzle.h"

class PuzzleRepository
{
private:
	std::vector<Model::Puzzle> puzzles;


public:
	PuzzleRepository(const std::string& puzzleFilePath);


	Model::Puzzle getPuzzle(int index) const;
	int getCount() const;
};
