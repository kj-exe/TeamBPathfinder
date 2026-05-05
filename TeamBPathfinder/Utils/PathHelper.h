#pragma once

#include <string>

namespace Utils
{
	/// <summary>
	/// Provides helper methods for resolving file paths used by the application.
	/// </summary>
	class PathHelper
	{
	public:
		/// <summary>
		/// Gets the runtime path for the puzzle file.
		/// </summary>
		/// <returns>The puzzle file path.</returns>
		static std::string getPuzzleFilePath();

		/// <summary>
		/// Gets the runtime path for the save game file.
		/// </summary>
		/// <returns>The save game file path.</returns>
		static std::string getSaveFilePath();

		/// <summary>
		/// Gets the project-level path for the scoreboard file.
		/// </summary>
		/// <returns>The scoreboard file path.</returns>
		static std::string getScoreboardFilePath();

		/// <summary>
		/// Gets the runtime path for the settings file.
		/// </summary>
		/// <returns>The settings file path.</returns>
		static std::string getSettingsFilePath();
	};
}
