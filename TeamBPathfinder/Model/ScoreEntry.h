#pragma once

#include <string>

namespace Model
{
	class ScoreEntry
	{
	private:
		std::string playerName;
		int puzzleNumber;
		int seconds;

	public:
		/// <summary>
		/// Initializes an empty score entry.
		/// </summary>
		ScoreEntry();

		/// <summary>
		/// Initializes a score entry with player, puzzle, and time information.
		/// </summary>
		/// <param name="playerName">The name of the player.</param>
		/// <param name="puzzleNumber">The puzzle number completed.</param>
		/// <param name="seconds">The completion time in seconds.</param>
		ScoreEntry(const std::string& playerName, int puzzleNumber, int seconds);

		/// <summary>
		/// Gets the player's name.
		/// </summary>
		/// <returns>The player's name.</returns>
		std::string getPlayerName() const;

		/// <summary>
		/// Gets the puzzle number associated with the score.
		/// </summary>
		/// <returns>The puzzle number.</returns>
		int getPuzzleNumber() const;

		/// <summary>
		/// Gets the completion time.
		/// </summary>
		/// <returns>The completion time in seconds.</returns>
		int getSeconds() const;
	};
}