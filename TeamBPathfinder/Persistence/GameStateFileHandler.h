#pragma once

#include <string>
#include "../Model/GameSnapshot.h"

class GameStateFileHandler
{
public:
    static bool saveGameState(const std::string& filePath, const GameSnapshot& snapshot);
    static bool loadGameState(const std::string& filePath, GameSnapshot& snapshot);
};