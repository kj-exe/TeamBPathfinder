#pragma once

#include "../Model/Puzzle.h"
#include "../Common/Constants.h"

namespace Engine
{
	/// <summary>
	/// Provides core game logic for managing the puzzle board, validating moves,
	/// and checking solution correctness.
	/// </summary>
	class GameEngine
	{
	private:
		int board[Constants::BOARD_ROW_SIZE][Constants::BOARD_COLUMN_SIZE];
		bool fixed[Constants::BOARD_ROW_SIZE][Constants::BOARD_COLUMN_SIZE];

	public:
		/// <summary>
		/// Initializes a new instance of the GameEngine and sets up an empty board.
		/// </summary>
		GameEngine();

		/// <summary>
		/// Resets the board to an empty state and clears all fixed flags.
		/// </summary>
		void initBoard();

		/// <summary>
		/// Sets a value in the board as fixed (non-editable).
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <param name="value">The value to assign.</param>
		void setFixed(int row, int col, int value);

		/// <summary>
		/// Loads a puzzle into the board by applying its fixed clue values.
		/// </summary>
		/// <param name="puzzle">The puzzle containing clue data.</param>
		void loadFromPuzzle(const Model::Puzzle& puzzle);

		/// <summary>
		/// Gets the value at the specified board cell.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>The value stored at the cell.</returns>
		int getValue(int row, int col) const;

		/// <summary>
		/// Determines whether a board cell is fixed (non-editable).
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>True if the cell is fixed; otherwise, false.</returns>
		bool isFixed(int row, int col) const;

		/// <summary>
		/// Checks whether placing a value in a cell is valid according to game rules.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <param name="value">The value to validate.</param>
		/// <returns>True if the move is valid; otherwise, false.</returns>
		bool isValidMove(int row, int col, int value);

		/// <summary>
		/// Determines whether the board is completely filled with non-zero values.
		/// </summary>
		/// <returns>True if the board is complete; otherwise, false.</returns>
		bool isBoardComplete();

		/// <summary>
		/// Validates whether the current board configuration matches the correct solution path.
		/// </summary>
		/// <returns>True if the solution is correct; otherwise, false.</returns>
		bool isSolutionCorrect();

		/// <summary>
		/// Attempts to place a value in a board cell.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <param name="value">The value to place.</param>
		/// <returns>True if the placement succeeds; otherwise, false.</returns>
		bool placeNumber(int row, int col, int value);

		/// <summary>
		/// Removes a value from a board cell if it is not fixed.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>True if the value was removed; otherwise, false.</returns>
		bool removeNumber(int row, int col);

		/// <summary>
		/// Sets a value in a cell only if the cell is editable.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <param name="value">The value to assign.</param>
		void setEditableValue(int row, int col, int value);
	};
}
