#pragma once

#include "../Model/Puzzle.h"

class GameEngine
{
private:
	int board[8][8];
	bool fixed[8][8];

public:
	GameEngine();

	void initBoard();
	void setFixed(int row, int col, int value);
	void loadFromPuzzle(const Puzzle& puzzle);

	int getValue(int row, int col) const;
	bool isFixed(int row, int col) const;
	bool isValidMove(int row, int col, int value);
	bool isBoardComplete();
	bool isSolutionCorrect();

	bool placeNumber(int row, int col, int value);
	bool removeNumber(int row, int col);
	void setEditableValue(int row, int col, int value);
};