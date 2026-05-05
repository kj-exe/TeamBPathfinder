#include "PuzzleRepository.h"
#include "../Persistence/PuzzleFileHandler.h"

#include <stdexcept>

namespace Model
{
	PuzzleRepository::PuzzleRepository(const std::string& puzzleFilePath)
	{
		this->puzzles = Persistence::PuzzleFileHandler::loadPuzzles(puzzleFilePath);

		if (this->puzzles.empty())
		{
			throw std::runtime_error("No puzzles were loaded.");
		}
	}

	Puzzle PuzzleRepository::getPuzzle(int index) const
	{
		if (index < 0 || index >= (int)this->puzzles.size())
		{
			throw std::out_of_range("Puzzle index is out of range.");
		}

		return this->puzzles[index];
	}

	int PuzzleRepository::getCount() const
	{
		return (int)this->puzzles.size();
	}
}
