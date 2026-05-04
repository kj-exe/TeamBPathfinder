#pragma once

class UserSettings
{
private:
    int cellColorRgb;
    int numberColorRgb;
    int fixedCellColorRgb;

public:
    UserSettings();

    int getCellColor() const;
    int getNumberColor() const;
    int getFixedCellColor() const;

    void setCellColor(int rgb);
    void setNumberColor(int rgb);
    void setFixedCellColor(int rgb);

    void resetToDefaults();
};