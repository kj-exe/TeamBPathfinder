#pragma once

#include <vector>

namespace Model
{
	/// <summary>
	/// Represents a snapshot of the game state, including all puzzle boards,
	/// solved statuses, elapsed times, and the currently active puzzle index.
	/// Used for saving and restoring game progress.
	/// </summary>
	class GameSnapshot
	{
	private:
		int currentPuzzleIndex;
		std::vector<std::vector<std::vector<int>>> boards;
		std::vector<bool> solved;
		std::vector<int> seconds;

	public:
		/// <summary>
		/// Initializes an empty snapshot with a default puzzle index.
		/// </summary>
		GameSnapshot();

		/// <summary>
		/// Initializes a snapshot with storage for the specified number of puzzles.
		/// </summary>
		/// <param name="puzzleCount">The number of puzzles to allocate storage for.</param>
		GameSnapshot(int puzzleCount);

		/// <summary>
		/// Gets the index of the currently active puzzle.
		/// </summary>
		/// <returns>The current puzzle index.</returns>
		int getCurrentPuzzleIndex() const;

		/// <summary>
		/// Sets the index of the currently active puzzle.
		/// </summary>
		/// <param name="index">The puzzle index to set.</param>
		void setCurrentPuzzleIndex(int index);

		/// <summary>
		/// Gets the total number of stored puzzle boards.
		/// </summary>
		/// <returns>The number of puzzle boards.</returns>
		int getBoardCount() const;

		/// <summary>
		/// Gets the value stored at a specific puzzle, row, and column.
		/// </summary>
		/// <param name="puzzleIndex">The puzzle index.</param>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>The value at the specified location.</returns>
		int getValue(int puzzleIndex, int row, int col) const;

		/// <summary>
		/// Sets the value at a specific puzzle, row, and column.
		/// </summary>
		/// <param name="puzzleIndex">The puzzle index.</param>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <param name="value">The value to store.</param>
		void setValue(int puzzleIndex, int row, int col, int value);

		/// <summary>
		/// Gets whether a puzzle has been solved.
		/// </summary>
		/// <param name="puzzleIndex">The puzzle index.</param>
		/// <returns>True if the puzzle is solved; otherwise, false.</returns>
		bool getSolved(int puzzleIndex) const;

		/// <summary>
		/// Sets the solved status of a puzzle.
		/// </summary>
		/// <param name="puzzleIndex">The puzzle index.</param>
		/// <param name="isSolved">The solved status to assign.</param>
		void setSolved(int puzzleIndex, bool isSolved);

		/// <summary>
		/// Gets the elapsed time for a puzzle in seconds.
		/// </summary>
		/// <param name="puzzleIndex">The puzzle index.</param>
		/// <returns>The elapsed time in seconds.</returns>
		int getSeconds(int puzzleIndex) const;

		/// <summary>
		/// Sets the elapsed time for a puzzle.
		/// </summary>
		/// <param name="puzzleIndex">The puzzle index.</param>
		/// <param name="sec">The elapsed time in seconds.</param>
		void setSeconds(int puzzleIndex, int sec);
	};
}