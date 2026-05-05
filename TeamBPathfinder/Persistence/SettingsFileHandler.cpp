#include "SettingsFileHandler.h"

#include <fstream>

namespace Persistence
{
	static const std::string CELL_COLOR_LABEL = "CellColor: ";
	static const std::string NUMBER_COLOR_LABEL = "NumberColor: ";
	static const std::string FIXED_CELL_COLOR_LABEL = "FixedCellColor: ";

	bool SettingsFileHandler::saveSettings(const std::string& filePath, const Model::UserSettings& settings)
	{
		std::ofstream file(filePath);

		if (!file)
		{
			return false;
		}

		file << CELL_COLOR_LABEL << settings.getCellColor() << std::endl;
		file << NUMBER_COLOR_LABEL << settings.getNumberColor() << std::endl;
		file << FIXED_CELL_COLOR_LABEL << settings.getFixedCellColor() << std::endl;

		return true;
	}

	bool SettingsFileHandler::loadSettings(const std::string& filePath, Model::UserSettings& settings)
	{
		std::ifstream file(filePath);

		if (!file)
		{
			return false;
		}

		std::string line;

		if (!std::getline(file, line))
		{
			return false;
		}

		settings.setCellColor(std::stoi(line.substr(CELL_COLOR_LABEL.length())));

		if (!std::getline(file, line))
		{
			return false;
		}

		settings.setNumberColor(std::stoi(line.substr(NUMBER_COLOR_LABEL.length())));

		if (!std::getline(file, line))
		{
			return false;
		}

		settings.setFixedCellColor(std::stoi(line.substr(FIXED_CELL_COLOR_LABEL.length())));

		return true;
	}
}
