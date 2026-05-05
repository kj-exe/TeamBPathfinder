#pragma once

#include <string>
#include <vector>
#include "Puzzle.h"

namespace Model
{
	/// <summary>
	/// Provides access to a collection of puzzles loaded from an external source.
	/// Responsible for storing and retrieving puzzles by index.
	/// </summary>
	class PuzzleRepository
	{
		std::vector<Puzzle> puzzles;

	public:
		/// <summary>
		/// Initializes the repository by loading puzzles from the specified file path.
		/// </summary>
		/// <param name="puzzleFilePath">The file path containing puzzle definitions.</param>
		/// <exception cref="std::runtime_error">
		/// Thrown if no puzzles are successfully loaded.
		/// </exception>
		PuzzleRepository(const std::string& puzzleFilePath);

		/// <summary>
		/// Retrieves a puzzle at the specified index.
		/// </summary>
		/// <param name="index">The zero-based index of the puzzle.</param>
		/// <returns>The puzzle at the given index.</returns>
		/// <exception cref="std::out_of_range">
		/// Thrown if the index is outside the valid range.
		/// </exception>
		Puzzle getPuzzle(int index) const;

		/// <summary>
		/// Gets the total number of puzzles in the repository.
		/// </summary>
		/// <returns>The number of stored puzzles.</returns>
		int getCount() const;
	};
}
