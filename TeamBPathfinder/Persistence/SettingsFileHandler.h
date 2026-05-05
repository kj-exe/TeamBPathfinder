#pragma once

#include <string>
#include "../Model/UserSettings.h"

namespace Persistence
{
	/// <summary>
	/// Provides functionality for saving and loading user display settings
	/// to and from a file.
	/// </summary>
	class SettingsFileHandler
	{
	public:
		/// <summary>
		/// Saves user settings to a file.
		/// </summary>
		/// <param name="filePath">The path of the file to save to.</param>
		/// <param name="settings">The user settings to persist.</param>
		/// <returns>True if the save operation succeeds; otherwise, false.</returns>
		static bool saveSettings(const std::string& filePath, const Model::UserSettings& settings);

		/// <summary>
		/// Loads user settings from a file.
		/// </summary>
		/// <param name="filePath">The path of the file to load from.</param>
		/// <param name="settings">The user settings object to populate.</param>
		/// <returns>True if the load operation succeeds; otherwise, false.</returns>
		static bool loadSettings(const std::string& filePath, Model::UserSettings& settings);
	};
}
