#include "UserSettings.h"

static const int DEFAULT_CELL_COLOR = (int)0xFFFFFFFF;
static const int DEFAULT_NUMBER_COLOR = (int)0xFF3C3C3C;
static const int DEFAULT_FIXED_CELL_COLOR = (int)0xFFDCDCDC;

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