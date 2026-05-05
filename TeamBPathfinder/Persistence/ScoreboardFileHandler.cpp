#include "ScoreboardFileHandler.h"

#include <fstream>
#include <sstream>

namespace Persistence
{
	static const char SCORE_FIELD_DELIMITER = '|';

	bool ScoreboardFileHandler::saveScoreboard(const std::string& filePath, const Model::Scoreboard& scoreboard)
	{
		std::ofstream file(filePath);

		if (!file)
		{
			return false;
		}

		const std::vector<Model::ScoreEntry>& scores = scoreboard.getScores();

		for (const Model::ScoreEntry& entry : scores)
		{
			file << entry.getPlayerName() << SCORE_FIELD_DELIMITER
				<< entry.getPuzzleNumber() << SCORE_FIELD_DELIMITER
				<< entry.getSeconds() << std::endl;
		}

		return true;
	}

	bool ScoreboardFileHandler::loadScoreboard(const std::string& filePath, Model::Scoreboard& scoreboard)
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

			if (!std::getline(ss, name, SCORE_FIELD_DELIMITER))
			{
				continue;
			}

			if (!std::getline(ss, puzzleText, SCORE_FIELD_DELIMITER))
			{
				continue;
			}

			if (!std::getline(ss, secondsText, SCORE_FIELD_DELIMITER))
			{
				continue;
			}

			int puzzleNumber = std::stoi(puzzleText);
			int seconds = std::stoi(secondsText);

			scoreboard.addScore(Model::ScoreEntry(name, puzzleNumber, seconds));
		}

		return true;
	}
}
