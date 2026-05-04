#pragma once

#include <string>
#include "../Model/Scoreboard.h"

class ScoreboardFileHandler
{
public:
	static bool saveScoreboard(const std::string& filePath, const Model::Scoreboard& scoreboard);
	static bool loadScoreboard(const std::string& filePath, Model::Scoreboard& scoreboard);
};