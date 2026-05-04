#include "Puzzle.h"

namespace Model
{
	Puzzle::Puzzle()
	{
		this->number = Constants::FIRST_PUZZLE_INDEX;

		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				solution[row][col] = Constants::EMPTY_VALUE;
			}
		}
	}

	Puzzle::Puzzle(int number)
	{
		this->number = number;

		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				solution[row][col] = Constants::EMPTY_VALUE;
			}
		}
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
}
