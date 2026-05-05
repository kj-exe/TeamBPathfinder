#pragma once

#include <string>
#include "../Model/GameSnapshot.h"

namespace Persistence
{
	/// <summary>
	/// Provides functionality for saving and loading game state data
	/// to and from a file.
	/// </summary>
	class GameStateFileHandler
	{
	public:
		/// <summary>
		/// Saves the current game state to a file.
		/// </summary>
		/// <param name="filePath">The path of the file to save to.</param>
		/// <param name="snapshot">The game state snapshot to persist.</param>
		/// <returns>True if the save operation succeeds; otherwise, false.</returns>
		static bool saveGameState(const std::string& filePath, const Model::GameSnapshot& snapshot);

		/// <summary>
		/// Loads game state data from a file into a snapshot.
		/// </summary>
		/// <param name="filePath">The path of the file to load from.</param>
		/// <param name="snapshot">The snapshot to populate with loaded data.</param>
		/// <returns>True if the load operation succeeds; otherwise, false.</returns>
		static bool loadGameState(const std::string& filePath, Model::GameSnapshot& snapshot);
	};
}