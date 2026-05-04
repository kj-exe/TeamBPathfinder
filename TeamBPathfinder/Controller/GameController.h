#pragma once

#include "../Engine/GameEngine.h"
#include "../Model/PuzzleRepository.h"
#include "../Model/GameSnapshot.h"
#include "../Common/Constants.h"
#include <vector>

namespace Controller
{
	/// <summary>
	/// Represents the result of a move, clear, or puzzle submission attempt.
	/// </summary>
	enum class MoveResult
	{
		Accepted,
		Cleared,
		InvalidNumber,
		InvalidCell,
		PuzzleSolved,
		PuzzleIncorrect,
		Incomplete
	};

	/// <summary>
	/// Controls game flow, puzzle switching, move validation, timer tracking, and game state snapshots.
	/// </summary>
	class GameController
	{
	private:
		Engine::GameEngine* engine;
		PuzzleRepository* repository;
		int currentPuzzleIndex;
		std::vector<std::vector<std::vector<int>>> savedBoards;
		std::vector<int> savedSeconds;
		std::vector<bool> solvedPuzzles;

		void saveCurrentBoardToMemory();
		void restoreBoardFromMemory(int puzzleIndex);

	public:
		/// <summary>
		/// Initializes the game controller with a game engine and puzzle repository.
		/// </summary>
		/// <param name="engine">The game engine used to manage the active board.</param>
		/// <param name="repository">The repository used to retrieve puzzle definitions.</param>
		GameController(Engine::GameEngine* engine, PuzzleRepository* repository);

		/// <summary>
		/// Starts the puzzle at the specified index after saving the current puzzle state.
		/// </summary>
		/// <param name="index">The zero-based index of the puzzle to start.</param>
		void startPuzzle(int index);

		/// <summary>
		/// Resets the current puzzle, clearing progress, solved status, and elapsed time.
		/// </summary>
		void resetCurrentPuzzle();

		/// <summary>
		/// Loads the first puzzle without saving a previous board state.
		/// </summary>
		void initializeFirstPuzzle();

		/// <summary>
		/// Fills the current puzzle with its solution values.
		/// </summary>
		void solveCurrentPuzzle();

		/// <summary>
		/// Gets the number of the current puzzle.
		/// </summary>
		/// <returns>The current puzzle number.</returns>
		int getCurrentPuzzleNumber() const;

		/// <summary>
		/// Gets the zero-based index of the current puzzle.
		/// </summary>
		/// <returns>The current puzzle index.</returns>
		int getCurrentPuzzleIndex() const;

		/// <summary>
		/// Gets the total number of available puzzles.
		/// </summary>
		/// <returns>The puzzle count.</returns>
		int getPuzzleCount() const;

		/// <summary>
		/// Attempts to place a value in the specified board cell.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <param name="value">The value to place.</param>
		/// <returns>The result of the move attempt.</returns>
		MoveResult attemptMove(int row, int col, int value);

		/// <summary>
		/// Attempts to clear the specified board cell.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>The result of the clear attempt.</returns>
		MoveResult clearCell(int row, int col);

		/// <summary>
		/// Submits the current puzzle for validation.
		/// </summary>
		/// <returns>The result of the puzzle submission.</returns>
		MoveResult submitPuzzle();

		/// <summary>
		/// Gets the value at the specified board cell.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>The value at the specified cell.</returns>
		int getValue(int row, int col) const;

		/// <summary>
		/// Checks whether the specified board cell is fixed.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="col">The column index.</param>
		/// <returns>True if the cell is fixed; otherwise, false.</returns>
		bool isFixed(int row, int col) const;

		/// <summary>
		/// Checks whether the current puzzle has already been solved.
		/// </summary>
		/// <returns>True if the current puzzle is solved; otherwise, false.</returns>
		bool isCurrentPuzzleSolved() const;

		/// <summary>
		/// Creates a snapshot of the current game state.
		/// </summary>
		/// <returns>A snapshot containing saved boards, solved statuses, timer values, and the current puzzle index.</returns>
		GameSnapshot getSnapshot();

		/// <summary>
		/// Loads the game state from a snapshot.
		/// </summary>
		/// <param name="snapshot">The snapshot to load from.</param>
		void loadFromSnapshot(const GameSnapshot& snapshot);

		/// <summary>
		/// Advances the timer for the current puzzle by one second.
		/// </summary>
		void tick();

		/// <summary>
		/// Resets the elapsed time for the current puzzle.
		/// </summary>
		void resetTimer();

		/// <summary>
		/// Gets the elapsed time for the current puzzle.
		/// </summary>
		/// <returns>The elapsed time in seconds.</returns>
		int getElapsedSeconds() const;
	};
}
