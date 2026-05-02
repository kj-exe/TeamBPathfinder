#pragma once

#include <string>

class GameController;

class GameStateFileHandler
{
public:
	static bool saveGameState(const std::string& filePath, GameController& controller);
	static bool loadGameState(const std::string& filePath, GameController& controller);
};