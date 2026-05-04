#include "ScoreEntry.h"
#include "../Common/Constants.h"

namespace Model
{
	ScoreEntry::ScoreEntry()
	{
		this->playerName = "";
		this->puzzleNumber = Constants::FIRST_PUZZLE_INDEX;
		this->seconds = 0;
	}


	ScoreEntry::ScoreEntry(const std::string& playerName, int puzzleNumber, int seconds)
	{
		this->playerName = playerName;
		this->puzzleNumber = puzzleNumber;
		this->seconds = seconds;
	}


	std::string ScoreEntry::getPlayerName() const
	{
		return this->playerName;
	}


	int ScoreEntry::getPuzzleNumber() const
	{
		return this->puzzleNumber;
	}


	int ScoreEntry::getSeconds() const
	{
		return this->seconds;
	}
}
