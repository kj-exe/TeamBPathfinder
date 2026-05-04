#pragma once

#include <vector>

#include "ScoreEntry.h"

class Scoreboard

{

private:

	std::vector<ScoreEntry> scores;



	void sortScores();

	void trimToTopTen();





public:

	void addScore(const ScoreEntry& entry);

	void clear();

	bool isHighScore(int seconds) const;

	const std::vector<ScoreEntry>& getScores() const;

};