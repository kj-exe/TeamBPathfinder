#include "SettingsFileHandler.h"

#include <fstream>

bool SettingsFileHandler::saveSettings(const std::string& filePath, const Model::UserSettings& settings)
{
    std::ofstream file(filePath);

    if (!file)
        return false;

    file << "CellColor: " << settings.getCellColor() << std::endl;
    file << "NumberColor: " << settings.getNumberColor() << std::endl;
    file << "FixedCellColor: " << settings.getFixedCellColor() << std::endl;

    return true;
}

bool SettingsFileHandler::loadSettings(const std::string& filePath, Model::UserSettings& settings)
{
    std::ifstream file(filePath);

    if (!file)
        return false;

    std::string line;

    if (!std::getline(file, line)) return false;
    settings.setCellColor(std::stoi(line.substr(line.find(":") + 1)));

    if (!std::getline(file, line)) return false;
    settings.setNumberColor(std::stoi(line.substr(line.find(":") + 1)));

    if (!std::getline(file, line)) return false;
    settings.setFixedCellColor(std::stoi(line.substr(line.find(":") + 1)));

    return true;
}