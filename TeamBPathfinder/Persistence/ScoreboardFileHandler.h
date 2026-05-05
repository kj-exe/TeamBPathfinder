#pragma once

#include <string>
#include "../Model/Scoreboard.h"

namespace Persistence
{
	/// <summary>
	/// Provides functionality for saving and loading scoreboard data
	/// to and from a file.
	/// </summary>
	class ScoreboardFileHandler
	{
	public:
		/// <summary>
		/// Saves scoreboard entries to a file.
		/// </summary>
		/// <param name="filePath">The path of the file to save to.</param>
		/// <param name="scoreboard">The scoreboard containing entries to persist.</param>
		/// <returns>True if the save operation succeeds; otherwise, false.</returns>
		static bool saveScoreboard(const std::string& filePath, const Model::Scoreboard& scoreboard);

		/// <summary>
		/// Loads scoreboard entries from a file into a scoreboard.
		/// </summary>
		/// <param name="filePath">The path of the file to load from.</param>
		/// <param name="scoreboard">The scoreboard to populate with loaded entries.</param>
		/// <returns>True if the load operation succeeds; otherwise, false.</returns>
		static bool loadScoreboard(const std::string& filePath, Model::Scoreboard& scoreboard);
	};
}
