#pragma once


#include <string>
#include <vector>
#include "../Model/Puzzle.h"


class PuzzleFileHandler

{
public:
	static std::vector<Puzzle> loadPuzzles(const std::string& filePath);

private:
	static void readClueLine(Puzzle& puzzle, const std::string& line);

	static void readSolutionRow(Puzzle& puzzle, const std::string& line, int row);
};
