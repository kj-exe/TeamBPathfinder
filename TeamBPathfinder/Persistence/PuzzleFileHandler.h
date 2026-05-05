#pragma once

#include <string>
#include <vector>
#include "../Model/Puzzle.h"

namespace Persistence
{
	/// <summary>
	/// Provides functionality for loading puzzle data from a file.
	/// </summary>
	class PuzzleFileHandler
	{
	private:
		static void readClueLine(Model::Puzzle& puzzle, const std::string& line);

		static void readSolutionRow(Model::Puzzle& puzzle, const std::string& line, int row);

	public:
		/// <summary>
		/// Loads puzzles from a file and returns them as a collection.
		/// </summary>
		/// <param name="filePath">The path to the puzzle file.</param>
		/// <returns>A vector of loaded puzzles.</returns>
		/// <exception cref="std::runtime_error">
		/// Thrown if the file cannot be opened.
		/// </exception>
		static std::vector<Model::Puzzle> loadPuzzles(const std::string& filePath);
	};
}
