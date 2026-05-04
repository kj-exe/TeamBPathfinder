#include "UserSettings.h"

namespace Model
{
	UserSettings::UserSettings()
	{
		resetToDefaults();
	}

	int UserSettings::getCellColor() const
	{
		return this->cellColorRgb;
	}

	int UserSettings::getNumberColor() const
	{
		return this->numberColorRgb;
	}

	int UserSettings::getFixedCellColor() const
	{
		return this->fixedCellColorRgb;
	}

	void UserSettings::setCellColor(int rgb)
	{
		this->cellColorRgb = rgb;
	}

	void UserSettings::setNumberColor(int rgb)
	{
		this->numberColorRgb = rgb;
	}

	void UserSettings::setFixedCellColor(int rgb)
	{
		this->fixedCellColorRgb = rgb;
	}

	void UserSettings::resetToDefaults()
	{
		this->cellColorRgb = DEFAULT_CELL_COLOR;
		this->numberColorRgb = DEFAULT_NUMBER_COLOR;
		this->fixedCellColorRgb = DEFAULT_FIXED_CELL_COLOR;
	}
}
