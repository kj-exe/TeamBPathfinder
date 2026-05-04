#pragma once

#include <string>
#include "../Model/UserSettings.h"

class SettingsFileHandler
{
public:
    static bool saveSettings(const std::string& filePath, const Model::UserSettings& settings);
    static bool loadSettings(const std::string& filePath, Model::UserSettings& settings);
};