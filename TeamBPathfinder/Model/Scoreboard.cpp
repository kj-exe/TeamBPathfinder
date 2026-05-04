#include "Scoreboard.h"

#include <algorithm>

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
	if (this->scores.size() > 10)
	{
		this->scores.resize(10);
	}
}

bool Scoreboard::isHighScore(int seconds) const
{
	if (this->scores.size() < 10)
	{
		return true; 
	}

	return seconds < this->scores.back().getSeconds();
}