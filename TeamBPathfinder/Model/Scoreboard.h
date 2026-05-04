#pragma once

#include <vector>
#include "ScoreEntry.h"

namespace Model
{
	/// <summary>
	/// Defines the available sorting modes for the scoreboard.
	/// </summary>
	enum class ScoreSortMode
	{
		Time,
		PuzzleThenTime
	};

	/// <summary>
	/// Manages a collection of high score entries, including adding scores,
	/// maintaining a top ranking, and providing sorted views.
	/// </summary>
	class Scoreboard
	{
	private:
		std::vector<ScoreEntry> scores;

		void sortScores();

		void trimToTopTen();

	public:
		/// <summary>
		/// Adds a new score entry to the scoreboard and updates rankings.
		/// </summary>
		/// <param name="entry">The score entry to add.</param>
		void addScore(const ScoreEntry& entry);

		/// <summary>
		/// Clears all scores from the scoreboard.
		/// </summary>
		void clear();

		/// <summary>
		/// Determines whether a given time qualifies as a high score.
		/// </summary>
		/// <param name="seconds">The time in seconds to evaluate.</param>
		/// <returns>True if the time is a high score; otherwise, false.</returns>
		bool isHighScore(int seconds) const;

		/// <summary>
		/// Gets the current list of stored score entries.
		/// </summary>
		/// <returns>A reference to the score collection.</returns>
		const std::vector<ScoreEntry>& getScores() const;

		/// <summary>
		/// Returns a sorted copy of the scores based on the specified mode.
		/// </summary>
		/// <param name="sortMode">The sorting mode to apply.</param>
		/// <returns>A sorted list of score entries.</returns>
		std::vector<ScoreEntry> getSortedScores(ScoreSortMode sortMode) const;
	};
}
