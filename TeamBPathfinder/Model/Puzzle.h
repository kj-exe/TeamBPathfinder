#pragma once

#include <vector>
#include "../Common/Constants.h"

namespace Model
{
	/// <summary>
	/// Represents a single clue in a puzzle, consisting of a fixed value
	/// at a specific row and column.
	/// </summary>
	struct Clue
	{
		int row;
		int col;
		int value;
	};

	/// <summary>
	/// Represents a puzzle, including its identifying number,
	/// fixed clue values, and complete solution grid.
	/// </summary>
	class Puzzle
	{
	private:
		int number;
		std::vector<Clue> clues;
		int solution[Constants::BOARD_ROW_SIZE][Constants::BOARD_COLUMN_SIZE];

	public:
		/// <summary>
		/// Initializes a puzzle with a default number and empty solution grid.
		/// </summary>
		Puzzle();

		/// <summary>
		/// Initializes a puzzle with a specified puzzle number.
		/// </summary>
		/// <param name="number">The identifier for the puzzle.</param>
		Puzzle(int number);

		/// <summary>
		/// Adds a fixed clue to the puzzle.
		/// </summary>
		/// <param name="row">The row index of the clue.</param>
		/// <param name="col">The column index of the clue.</param>
		/// <param name="value">The value of the clue.</param>
		void addClue(int row, int col, int value);

		/// <summary>
		/// Sets a value in the solution grid.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <param name="value">The value to assign.</param>
		void setSolution(int row, int col, int value);

		/// <summary>
		/// Gets the puzzle number.
		/// </summary>
		/// <returns>The puzzle identifier.</returns>
		int getNumber() const;

		/// <summary>
		/// Gets the list of fixed clues for the puzzle.
		/// </summary>
		/// <returns>A reference to the collection of clues.</returns>
		const std::vector<Clue>& getClues() const;

		/// <summary>
		/// Gets a value from the solution grid at the specified position.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>The solution value at the specified location.</returns>
		int getSolutionValue(int row, int col) const;
	};
}