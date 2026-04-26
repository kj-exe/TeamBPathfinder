#include "GameEngine.h"

#include <cmath>

GameEngine::GameEngine()
{
	initBoard();
}

void GameEngine::initBoard()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			board[i][j] = 0;
			fixed[i][j] = false;
		}
	}
}

void GameEngine::loadFromPuzzle(const Puzzle& puzzle)
{
	initBoard();
	const std::vector<Clue>& clues = puzzle.getClues();
	for (size_t i = 0; i < clues.size(); i++)
	{
		setFixed(clues[i].row, clues[i].col, clues[i].value);
	}
}

void GameEngine::setFixed(int row, int col, int value)
{
	board[row][col] = value;
	fixed[row][col] = true;
}

int GameEngine::getValue(int row, int col)
{
	return board[row][col];
}

bool GameEngine::isFixed(int row, int col)
{
	return fixed[row][col];
}

bool GameEngine::isValidMove(int row, int col, int value)
{
	if (value < 1 || value > 64)
		return false;
	if (fixed[row][col])
		return false;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (board[i][j] == value)
				return false;
	return true;
}

bool GameEngine::placeNumber(int row, int col, int value)
{
	if (isValidMove(row, col, value) && !isFixed(row, col))
	{
		board[row][col] = value;
		return true;
	}
	return false;
}

bool GameEngine::removeNumber(int row, int col)
{
	if (!isFixed(row, col))
	{
		board[row][col] = 0;
		return true;
	}
	return false;
}

bool GameEngine::isBoardComplete()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == 0)
				return false;
		}
	}
	return true;
}

bool GameEngine::isSolutionCorrect()
{
	int posRow[65], posCol[65];
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			int val = board[i][j];
			if (val < 1 || val > 64)
				return false;
			posRow[val] = i;
			posCol[val] = j;
		}

	for (int n = 1; n < 64; n++)
	{
		int rowDiff = abs(posRow[n] - posRow[n + 1]);
		int colDiff = abs(posCol[n] - posCol[n + 1]);
		if (rowDiff + colDiff != 1)
			return false;
	}
	return true;
}
