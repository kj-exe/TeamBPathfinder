#pragma once

namespace Constants
{
	/// <summary>
	/// The number of rows in the game board.
	/// </summary>
	static const int BOARD_ROW_SIZE = 8;

	/// <summary>
	/// The number of columns in the game board.
	/// </summary>
	static const int BOARD_COLUMN_SIZE = 8;

	/// <summary>
	/// Represents an empty cell value on the board.
	/// </summary>
	static const int EMPTY_VALUE = 0;

	/// <summary>
	/// The minimum valid value that can be placed on the board.
	/// </summary>
	static const int MIN_VALUE = 1;

	/// <summary>
	/// The maximum valid value that can be placed on the board.
	/// </summary>
	static const int MAX_VALUE = 64;

	/// <summary>
	/// The size of the position tracking arrays used for solution validation.
	/// </summary>
	static const int POSITION_ARRAY_SIZE = MAX_VALUE + 1;

	/// <summary>
	/// The required distance between consecutive values in a valid solution.
	/// </summary>
	static const int REQUIRED_ADJACENT_DISTANCE = 1;

	/// <summary>
	/// The index of the first puzzle in the collection.
	/// </summary>
	static const int FIRST_PUZZLE_INDEX = 0;

	/// <summary>
	/// Offset used to convert a zero-based index to a one-based display number.
	/// </summary>
	static const int DISPLAY_NUMBER_OFFSET = 1;

	/// <summary>
	/// The number of seconds added to the timer per tick.
	/// </summary>
	static const int TIMER_INCREMENT_SECONDS = 1;
}