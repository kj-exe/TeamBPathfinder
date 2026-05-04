#include "Scoreboard.h"
#include "../Common/Constants.h"

#include <algorithm>

namespace Model
{
	void Scoreboard::addScore(const ScoreEntry& entry)
	{
		this->scores.push_back(entry);
		this->sortScores();
		this->trimToTopTen();
	}

	void Scoreboard::clear()
	{
		this->scores.clear();
	}

	const std::vector<ScoreEntry>& Scoreboard::getScores() const
	{
		return this->scores;
	}

	void Scoreboard::sortScores()
	{
		std::sort(
			this->scores.begin(),
			this->scores.end(),
			[](const ScoreEntry& first, const ScoreEntry& second)
			{
				return first.getSeconds() < second.getSeconds();
			}
		);
	}

	void Scoreboard::trimToTopTen()
	{
		if (this->scores.size() > Constants::MAX_SCORE_ENTRIES)
		{
			this->scores.resize(Constants::MAX_SCORE_ENTRIES);
		}
	}

	bool Scoreboard::isHighScore(int seconds) const
	{
		if (this->scores.size() < Constants::MAX_SCORE_ENTRIES)
		{
			return true;
		}

		return seconds < this->scores.back().getSeconds();
	}

	std::vector<ScoreEntry> Scoreboard::getSortedScores(ScoreSortMode mode) const
	{
		std::vector<ScoreEntry> sorted = this->scores;

		std::sort(
			sorted.begin(),
			sorted.end(),
			[mode](const ScoreEntry& first, const ScoreEntry& second)
			{
				if (mode == ScoreSortMode::PuzzleThenTime)
				{
					if (first.getPuzzleNumber() == second.getPuzzleNumber())
					{
						return first.getSeconds() < second.getSeconds();
					}
					return first.getPuzzleNumber() < second.getPuzzleNumber();
				}

				return first.getSeconds() < second.getSeconds();
			}
		);

		return sorted;
	}
}
