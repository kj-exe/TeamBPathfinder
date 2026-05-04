#include "ScoreboardFileHandler.h"

#include <fstream>
#include <sstream>

bool ScoreboardFileHandler::saveScoreboard(const std::string& filePath, const Scoreboard& scoreboard)
{
	std::ofstream file(filePath);

	if (!file)
	{
		return false;
	}

	const std::vector<ScoreEntry>& scores = scoreboard.getScores();

	for (const ScoreEntry& entry : scores)
	{
		file << entry.getPlayerName() << "|"
			<< entry.getPuzzleNumber() << "|"
			<< entry.getSeconds() << std::endl;
	}

	return true;
}

bool ScoreboardFileHandler::loadScoreboard(const std::string& filePath, Scoreboard& scoreboard)
{
	std::ifstream file(filePath);

	if (!file)
	{
		return false;
	}

	scoreboard.clear();

	std::string line;

	while (std::getline(file, line))
	{
		std::stringstream ss(line);

		std::string name;
		std::string puzzleText;
		std::string secondsText;

		if (!std::getline(ss, name, '|'))
		{
			continue;
		}

		if (!std::getline(ss, puzzleText, '|'))
		{
			continue;
		}

		if (!std::getline(ss, secondsText, '|'))
		{
			continue;
		}

		int puzzleNumber = std::stoi(puzzleText);
		int seconds = std::stoi(secondsText);

		scoreboard.addScore(ScoreEntry(name, puzzleNumber, seconds));
	}

	return true;
}
