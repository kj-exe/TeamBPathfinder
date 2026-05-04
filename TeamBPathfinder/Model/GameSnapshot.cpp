#include "GameSnapshot.h"
#include "../Common/Constants.h"

namespace Model
{
	GameSnapshot::GameSnapshot()
	{
		this->currentPuzzleIndex = Constants::FIRST_PUZZLE_INDEX;
	}

	GameSnapshot::GameSnapshot(int puzzleCount)
	{
		this->currentPuzzleIndex = Constants::FIRST_PUZZLE_INDEX;

		this->boards.resize(
			puzzleCount,
			std::vector<std::vector<int>>(
				Constants::BOARD_ROW_SIZE,
				std::vector<int>(Constants::BOARD_COLUMN_SIZE, Constants::EMPTY_VALUE)
			)
		);

		this->solved.resize(puzzleCount, false);
		this->seconds.resize(puzzleCount, 0);
	}

	int GameSnapshot::getCurrentPuzzleIndex() const
	{
		return this->currentPuzzleIndex;
	}

	void GameSnapshot::setCurrentPuzzleIndex(int index)
	{
		this->currentPuzzleIndex = index;
	}

	int GameSnapshot::getBoardCount() const
	{
		return (int)this->boards.size();
	}

	int GameSnapshot::getValue(int puzzleIndex, int row, int col) const
	{
		return this->boards[puzzleIndex][row][col];
	}

	void GameSnapshot::setValue(int puzzleIndex, int row, int col, int value)
	{
		this->boards[puzzleIndex][row][col] = value;
	}

	bool GameSnapshot::getSolved(int puzzleIndex) const
	{
		return this->solved[puzzleIndex];
	}

	void GameSnapshot::setSolved(int puzzleIndex, bool isSolved)
	{
		this->solved[puzzleIndex] = isSolved;
	}

	int GameSnapshot::getSeconds(int puzzleIndex) const
	{
		return this->seconds[puzzleIndex];
	}

	void GameSnapshot::setSeconds(int puzzleIndex, int sec)
	{
		this->seconds[puzzleIndex] = sec;
	}
}
