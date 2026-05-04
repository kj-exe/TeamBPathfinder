#pragma once

#include <string>

namespace Utils
{
	class PathHelper
	{
	public:
		static std::string getPuzzleFilePath();

		static std::string getSaveFilePath();

		static std::string getScoreboardFilePath();
		static std::string getSettingsFilePath();
	};
}
