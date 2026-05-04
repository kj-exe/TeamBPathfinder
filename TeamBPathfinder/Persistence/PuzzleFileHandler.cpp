#include "PuzzleFileHandler.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

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
			continue;

		if (line.find("PuzzleNumber:") == 0)
		{
			int puzzleNumber = std::stoi(line.substr(line.find(":") + 1));
			currentPuzzle = Model::Puzzle(puzzleNumber);

			hasCurrentPuzzle = true;
			readingClues = false;
			readingSolution = false;
			solutionRow = 0;
		}
		else if (line == "Clues:")
		{
			readingClues = true;
			readingSolution = false;
		}
		else if (line == "Solution:")
		{
			readingClues = false;
			readingSolution = true;
			solutionRow = 0;
		}
		else if (line == "EndPuzzle")
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

	for (int col = 0; col < 8; col++)
	{
		int value;
		ss >> value;
		puzzle.setSolution(row, col, value);
	}
}
