#include "GameStateFileHandler.h"
#include "../Common/Constants.h"

#include <fstream>
#include <limits>

namespace Persistence
{
	static const std::string CURRENT_PUZZLE_INDEX_LABEL = "CurrentPuzzleIndex: ";
	static const std::string PUZZLE_COUNT_LABEL = "PuzzleCount: ";
	static const std::string PUZZLE_INDEX_LABEL = "PuzzleIndex: ";
	static const std::string BOARD_LABEL = "Board:";
	static const std::string SOLVED_LABEL = "Solved: ";
	static const std::string SECONDS_LABEL = "Seconds: ";
	static const char SPACE_SEPARATOR = ' ';

	bool GameStateFileHandler::saveGameState(const std::string& filePath, const Model::GameSnapshot& snapshot)
	{
		std::ofstream file(filePath);

		if (!file)
		{
			return false;
		}

		file << CURRENT_PUZZLE_INDEX_LABEL << snapshot.getCurrentPuzzleIndex() << std::endl;
		file << PUZZLE_COUNT_LABEL << snapshot.getBoardCount() << std::endl;

		for (int puzzleIndex = 0; puzzleIndex < snapshot.getBoardCount(); puzzleIndex++)
		{
			file << PUZZLE_INDEX_LABEL << puzzleIndex << std::endl;
			file << BOARD_LABEL << std::endl;

			for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
			{
				for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
				{
					file << snapshot.getValue(puzzleIndex, row, col);

					if (col < Constants::BOARD_COLUMN_SIZE - Constants::DISPLAY_NUMBER_OFFSET)
					{
						file << SPACE_SEPARATOR;
					}
				}
				file << std::endl;
			}

			file << SOLVED_LABEL << (snapshot.getSolved(puzzleIndex) ? 1 : 0) << std::endl;
			file << SECONDS_LABEL << snapshot.getSeconds(puzzleIndex) << std::endl;
		}

		return true;
	}

	bool GameStateFileHandler::loadGameState(const std::string& filePath, Model::GameSnapshot& snapshot)
	{
		std::ifstream file(filePath);

		if (!file)
		{
			return false;
		}

		std::string line;

		std::getline(file, line);
		int currentPuzzleIndex = std::stoi(line.substr(CURRENT_PUZZLE_INDEX_LABEL.length()));

		std::getline(file, line);
		int boardCount = std::stoi(line.substr(PUZZLE_COUNT_LABEL.length()));

		if (boardCount != snapshot.getBoardCount())
		{
			return false;
		}

		if (currentPuzzleIndex < 0 || currentPuzzleIndex >= boardCount)
		{
			return false;
		}

		snapshot.setCurrentPuzzleIndex(currentPuzzleIndex);

		for (int i = 0; i < boardCount; i++)
		{
			std::getline(file, line);
			int puzzleIndex = std::stoi(line.substr(PUZZLE_INDEX_LABEL.length()));

			std::getline(file, line);

			if (puzzleIndex < 0 || puzzleIndex >= boardCount)
			{
				return false;
			}

			for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
			{
				for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
				{
					int value;

					if (!(file >> value))
					{
						return false;
					}

					snapshot.setValue(puzzleIndex, row, col, value);
				}
			}

			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			std::getline(file, line);
			int solvedFlag = std::stoi(line.substr(SOLVED_LABEL.length()));
			snapshot.setSolved(puzzleIndex, solvedFlag == 1);

			std::getline(file, line);
			int seconds = std::stoi(line.substr(SECONDS_LABEL.length()));
			snapshot.setSeconds(puzzleIndex, seconds);
		}

		return true;
	}
}
