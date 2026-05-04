#pragma once

namespace Model
{
	/// <summary>
	/// Stores user-configurable display settings for the game board.
	/// </summary>
	class UserSettings
	{
	private:
		static const int DEFAULT_CELL_COLOR = (int)0xFFFFFFFF;
		static const int DEFAULT_NUMBER_COLOR = (int)0xFF3C3C3C;
		static const int DEFAULT_FIXED_CELL_COLOR = (int)0xFFDCDCDC;

		int cellColorRgb;
		int numberColorRgb;
		int fixedCellColorRgb;

	public:
		/// <summary>
		/// Initializes the user settings with default colors.
		/// </summary>
		UserSettings();

		/// <summary>
		/// Gets the editable cell color.
		/// </summary>
		/// <returns>The editable cell color as an RGB integer.</returns>
		int getCellColor() const;

		/// <summary>
		/// Gets the number color.
		/// </summary>
		/// <returns>The number color as an RGB integer.</returns>
		int getNumberColor() const;

		/// <summary>
		/// Gets the fixed cell color.
		/// </summary>
		/// <returns>The fixed cell color as an RGB integer.</returns>
		int getFixedCellColor() const;

		/// <summary>
		/// Sets the editable cell color.
		/// </summary>
		/// <param name="rgb">The RGB color value to assign.</param>
		void setCellColor(int rgb);

		/// <summary>
		/// Sets the number color.
		/// </summary>
		/// <param name="rgb">The RGB color value to assign.</param>
		void setNumberColor(int rgb);

		/// <summary>
		/// Sets the fixed cell color.
		/// </summary>
		/// <param name="rgb">The RGB color value to assign.</param>
		void setFixedCellColor(int rgb);

		/// <summary>
		/// Restores all settings to their default values.
		/// </summary>
		void resetToDefaults();
	};
}