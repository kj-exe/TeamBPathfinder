#pragma once

#include <string>
#include "../Model/Scoreboard.h"

class ScoreboardFileHandler
{
public:
	static bool saveScoreboard(const std::string& filePath, const Scoreboard& scoreboard);
	static bool loadScoreboard(const std::string& filePath, Scoreboard& scoreboard);
};