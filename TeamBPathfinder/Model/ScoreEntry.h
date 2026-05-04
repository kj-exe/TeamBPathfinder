#pragma once

#include <string>

class ScoreEntry
{
private:
	std::string playerName;
	int puzzleNumber;
	int seconds;

public:
	ScoreEntry();
	ScoreEntry(const std::string& playerName, int puzzleNumber, int seconds);

	std::string getPlayerName() const;
	int getPuzzleNumber() const;
	int getSeconds() const;
};