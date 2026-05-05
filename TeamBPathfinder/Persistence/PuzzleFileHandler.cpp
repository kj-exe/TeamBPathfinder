#include "PuzzleFileHandler.h"
#include "../Common/Constants.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Persistence
{
	static const std::string PUZZLE_NUMBER_LABEL = "PuzzleNumber:";
	static const std::string CLUES_LABEL = "Clues:";
	static const std::string SOLUTION_LABEL = "Solution:";
	static const std::string END_PUZZLE_LABEL = "EndPuzzle";

	std::vector<Model::Puzzle> PuzzleFileHandler::loadPuzzles(const std::string& filePath)
	{
		std::ifstream file(filePath);

		if (!file)
		{
			throw std::runtime_error("Could not open puzzle file: " + filePath);
		}

		std::vector<Model::Puzzle> puzzles;
		std::string line;
		Model::Puzzle currentPuzzle;
		bool hasCurrentPuzzle = false;
		bool readingClues = false;
		bool readingSolution = false;
		int solutionRow = 0;

		while (std::getline(file, line))
		{
			if (line.empty())
			{
				continue;
			}

			if (line.find(PUZZLE_NUMBER_LABEL) == 0)
			{
				int puzzleNumber = std::stoi(line.substr(PUZZLE_NUMBER_LABEL.length()));
				currentPuzzle = Model::Puzzle(puzzleNumber);

				hasCurrentPuzzle = true;
				readingClues = false;
				readingSolution = false;
				solutionRow = 0;
			}
			else if (line == CLUES_LABEL)
			{
				readingClues = true;
				readingSolution = false;
			}
			else if (line == SOLUTION_LABEL)
			{
				readingClues = false;
				readingSolution = true;
				solutionRow = 0;
			}
			else if (line == END_PUZZLE_LABEL)
			{
				if (hasCurrentPuzzle)
				{
					puzzles.push_back(currentPuzzle);
				}

				hasCurrentPuzzle = false;
				readingClues = false;
				readingSolution = false;
				solutionRow = 0;
			}
			else if (readingClues)
			{
				readClueLine(currentPuzzle, line);
			}
			else if (readingSolution)
			{
				readSolutionRow(currentPuzzle, line, solutionRow);
				solutionRow++;
			}
		}

		return puzzles;
	}

	void PuzzleFileHandler::readClueLine(Model::Puzzle& puzzle, const std::string& line)
	{
		std::stringstream ss(line);

		int row;
		int col;
		int value;

		ss >> row >> col >> value;

		puzzle.addClue(row, col, value);
	}

	void PuzzleFileHandler::readSolutionRow(Model::Puzzle& puzzle, const std::string& line, int row)
	{
		std::stringstream ss(line);

		for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
		{
			int value;
			ss >> value;
			puzzle.setSolution(row, col, value);
		}
	}
}
