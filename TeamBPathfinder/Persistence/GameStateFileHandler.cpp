#include "GameStateFileHandler.h"

#include "../Controller/GameController.h"

#include <fstream>
#include <limits>

bool GameStateFileHandler::saveGameState(const std::string& filePath, GameController& controller)
{
	controller.saveCurrentBoardToMemory();

	std::ofstream file(filePath);

	if (!file)
	{
		return false;
	}

	file << "CurrentPuzzleIndex: " << controller.getCurrentPuzzleIndex() << std::endl;
	file << "PuzzleCount: " << controller.getSavedBoardCount() << std::endl;

	for (int puzzleIndex = 0; puzzleIndex < controller.getSavedBoardCount(); puzzleIndex++)
	{
		file << "PuzzleIndex: " << puzzleIndex << std::endl;
		file << "Board:" << std::endl;

		for (int row = 0; row < 8; row++)
		{
			for (int col = 0; col < 8; col++)
			{
				file << controller.getSavedBoardValue(puzzleIndex, row, col);

				if (col < 7)
				{
					file << " ";
				}
			}

			file << std::endl;
		}
	}

	return true;
}
bool GameStateFileHandler::loadGameState(const std::string& filePath, GameController& controller)
{
	std::ifstream file(filePath);

	if (!file)
	{
		return false;
	}


	std::string line;

	std::getline(file, line);
	int currentPuzzleIndex = std::stoi(line.substr(line.find(":") + 1));

	std::getline(file, line);
	int boardCount = std::stoi(line.substr(line.find(":") + 1));

	if (currentPuzzleIndex < 0 || currentPuzzleIndex >= controller.getPuzzleCount())
	{
		return false;
	}

	if (boardCount != controller.getSavedBoardCount())
	{
		return false;
	}

	for (int i = 0; i < boardCount; i++)
	{
		std::getline(file, line);
		int puzzleIndex = std::stoi(line.substr(line.find(":") + 1));

		std::getline(file, line);

		if (puzzleIndex < 0 || puzzleIndex >= controller.getSavedBoardCount())
		{
			return false;
		}

		for (int row = 0; row < 8; row++)
		{
			for (int col = 0; col < 8; col++)
			{
				int value;

				if (!(file >> value))
				{
					return false;
				}

				controller.setSavedBoardValue(puzzleIndex, row, col, value);
			}
		}
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	controller.loadPuzzleFromMemory(currentPuzzleIndex);

	return true;
}