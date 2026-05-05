#include "GameEngine.h"

#include <cmath>

namespace Engine
{
	GameEngine::GameEngine()
	{
		initBoard();
	}

	void GameEngine::initBoard()
	{
		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				board[row][col] = Constants::EMPTY_VALUE;
				fixed[row][col] = false;
			}
		}
	}

	void GameEngine::loadFromPuzzle(const Model::Puzzle& puzzle)
	{
		initBoard();
		const std::vector<Model::Clue>& clues = puzzle.getClues();
		for (size_t i = 0; i < clues.size(); i++)
		{
			setFixed(clues[i].row, clues[i].col, clues[i].value);
		}
	}

	void GameEngine::setFixed(const int row, const int col, int value)
	{
		board[row][col] = value;
		fixed[row][col] = true;
	}

	int GameEngine::getValue(int row, int col) const
	{
		return board[row][col];
	}

	bool GameEngine::isFixed(int row, int col) const
	{
		return fixed[row][col];
	}

	bool GameEngine::isValidMove(int row, int col, int value)
	{
		if (value < Constants::MIN_VALUE || value > Constants::MAX_VALUE)
		{
			return false;
		}

		if (fixed[row][col])
		{
			return false;
		}

		for (int currentRow = 0; currentRow < Constants::BOARD_ROW_SIZE; currentRow++)
		{
			for (int currentCol = 0; currentCol < Constants::BOARD_COLUMN_SIZE; currentCol++)
			{
				if (board[currentRow][currentCol] == value)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool GameEngine::placeNumber(int row, int col, int value)
	{
		if (isValidMove(row, col, value) && !isFixed(row, col))
		{
			board[row][col] = value;
			return true;
		}
		return false;
	}

	bool GameEngine::removeNumber(int row, int col)
	{
		if (!isFixed(row, col))
		{
			board[row][col] = Constants::EMPTY_VALUE;
			return true;
		}
		return false;
	}

	bool GameEngine::isBoardComplete()
	{
		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				if (board[row][col] == Constants::EMPTY_VALUE)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool GameEngine::isSolutionCorrect()
	{
		int posRow[Constants::POSITION_ARRAY_SIZE];
		int posCol[Constants::POSITION_ARRAY_SIZE];

		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				int val = board[row][col];

				if (val < Constants::MIN_VALUE || val > Constants::MAX_VALUE)
				{
					return false;
				}

				posRow[val] = row;
				posCol[val] = col;
			}
		}

		for (int number = Constants::MIN_VALUE; number < Constants::MAX_VALUE; number++)
		{
			int rowDiff = abs(posRow[number] - posRow[number + Constants::DISPLAY_NUMBER_OFFSET]);
			int colDiff = abs(posCol[number] - posCol[number + Constants::DISPLAY_NUMBER_OFFSET]);

			if (rowDiff + colDiff != Constants::REQUIRED_ADJACENT_DISTANCE)
			{
				return false;
			}
		}

		return true;
	}

	void GameEngine::setEditableValue(int row, int col, int value)
	{
		if (!fixed[row][col])
		{
			board[row][col] = value;
		}
	}
}
