#pragma once


#include <string>
#include <vector>
#include "../Model/Puzzle.h"


class PuzzleFileHandler

{
public:
	static std::vector<Model::Puzzle> loadPuzzles(const std::string& filePath);

private:
	static void readClueLine(Model::Puzzle& puzzle, const std::string& line);

	static void readSolutionRow(Model::Puzzle& puzzle, const std::string& line, int row);
};
