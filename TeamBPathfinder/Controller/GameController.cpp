#include "GameController.h"
#include "../Common/Constants.h"

namespace Controller
{
	GameController::GameController(Engine::GameEngine* engine, Model::PuzzleRepository* repository)
	{
		this->engine = engine;
		this->repository = repository;
		this->currentPuzzleIndex = Constants::FIRST_PUZZLE_INDEX;

		int puzzleCount = repository->getCount();

		this->savedBoards.resize(
			puzzleCount,
			std::vector<std::vector<int>>(
				Constants::BOARD_ROW_SIZE,
				std::vector<int>(Constants::BOARD_COLUMN_SIZE, Constants::EMPTY_VALUE)
			)
		);

		this->savedSeconds.resize(puzzleCount, 0);
		this->solvedPuzzles.resize(puzzleCount, false);
	}

	void GameController::startPuzzle(int index)
	{
		this->saveCurrentBoardToMemory();
		this->currentPuzzleIndex = index;

		Model::Puzzle puzzle = this->repository->getPuzzle(index);
		this->engine->loadFromPuzzle(puzzle);

		this->restoreBoardFromMemory(index);
	}

	void GameController::resetCurrentPuzzle()
	{
		this->savedSeconds[this->currentPuzzleIndex] = 0;
		this->solvedPuzzles[this->currentPuzzleIndex] = false;

		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				this->savedBoards[this->currentPuzzleIndex][row][col] = Constants::EMPTY_VALUE;
			}
		}

		Model::Puzzle puzzle = this->repository->getPuzzle(this->currentPuzzleIndex);
		this->engine->loadFromPuzzle(puzzle);
	}

	void GameController::initializeFirstPuzzle()
	{
		this->currentPuzzleIndex = Constants::FIRST_PUZZLE_INDEX;

		Model::Puzzle puzzle = this->repository->getPuzzle(Constants::FIRST_PUZZLE_INDEX);
		this->engine->loadFromPuzzle(puzzle);

		this->restoreBoardFromMemory(Constants::FIRST_PUZZLE_INDEX);
	}

	void GameController::solveCurrentPuzzle()
	{
		Model::Puzzle puzzle = this->repository->getPuzzle(this->currentPuzzleIndex);

		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				if (!this->engine->isFixed(row, col))
				{
					this->engine->removeNumber(row, col);
					this->engine->placeNumber(row, col, puzzle.getSolutionValue(row, col));
				}
			}
		}

		this->saveCurrentBoardToMemory();
	}

	int GameController::getCurrentPuzzleNumber() const
	{
		return this->currentPuzzleIndex + Constants::DISPLAY_NUMBER_OFFSET;
	}

	int GameController::getCurrentPuzzleIndex() const
	{
		return this->currentPuzzleIndex;
	}

	int GameController::getPuzzleCount() const
	{
		return this->repository->getCount();
	}

	MoveResult GameController::attemptMove(int row, int col, int value)
	{
		if (this->engine->isFixed(row, col))
		{
			return MoveResult::InvalidCell;
		}

		this->engine->removeNumber(row, col);

		if (!this->engine->placeNumber(row, col, value))
		{
			return MoveResult::InvalidNumber;
		}

		return MoveResult::Accepted;
	}

	MoveResult GameController::clearCell(int row, int col)
	{
		if (this->engine->isFixed(row, col))
		{
			return MoveResult::InvalidCell;
		}

		this->engine->removeNumber(row, col);
		return MoveResult::Cleared;
	}

	MoveResult GameController::submitPuzzle()
	{
		if (!this->engine->isBoardComplete())
		{
			return MoveResult::Incomplete;
		}

		if (this->engine->isSolutionCorrect())
		{
			this->solvedPuzzles[this->currentPuzzleIndex] = true;
			this->saveCurrentBoardToMemory();
			return MoveResult::PuzzleSolved;
		}

		return MoveResult::PuzzleIncorrect;
	}

	int GameController::getValue(int row, int col) const
	{
		return this->engine->getValue(row, col);
	}

	bool GameController::isFixed(int row, int col) const
	{
		return this->engine->isFixed(row, col);
	}

	bool GameController::isCurrentPuzzleSolved() const
	{
		return this->solvedPuzzles[this->currentPuzzleIndex];
	}

	void GameController::saveCurrentBoardToMemory()
	{
		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				this->savedBoards[this->currentPuzzleIndex][row][col] =
					this->engine->getValue(row, col);
			}
		}
	}

	void GameController::restoreBoardFromMemory(int puzzleIndex)
	{
		for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
		{
			for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
			{
				int value = this->savedBoards[puzzleIndex][row][col];

				if (!this->engine->isFixed(row, col))
				{
					this->engine->setEditableValue(row, col, value);
				}
			}
		}
	}

	Model::GameSnapshot GameController::getSnapshot()
	{
		this->saveCurrentBoardToMemory();

		int puzzleCount = this->getPuzzleCount();
		Model::GameSnapshot snapshot(puzzleCount);

		snapshot.setCurrentPuzzleIndex(this->currentPuzzleIndex);

		for (int puzzleIndex = 0; puzzleIndex < puzzleCount; puzzleIndex++)
		{
			for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
			{
				for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
				{
					snapshot.setValue(
						puzzleIndex,
						row,
						col,
						this->savedBoards[puzzleIndex][row][col]
					);
				}
			}

			snapshot.setSolved(puzzleIndex, this->solvedPuzzles[puzzleIndex]);
			snapshot.setSeconds(puzzleIndex, this->savedSeconds[puzzleIndex]);
		}

		return snapshot;
	}

	void GameController::loadFromSnapshot(const Model::GameSnapshot& snapshot)
	{
		int puzzleCount = this->getPuzzleCount();

		if (snapshot.getBoardCount() != puzzleCount)
		{
			return;
		}

		for (int puzzleIndex = 0; puzzleIndex < puzzleCount; puzzleIndex++)
		{
			for (int row = 0; row < Constants::BOARD_ROW_SIZE; row++)
			{
				for (int col = 0; col < Constants::BOARD_COLUMN_SIZE; col++)
				{
					this->savedBoards[puzzleIndex][row][col] =
						snapshot.getValue(puzzleIndex, row, col);
				}
			}

			this->solvedPuzzles[puzzleIndex] = snapshot.getSolved(puzzleIndex);
			this->savedSeconds[puzzleIndex] = snapshot.getSeconds(puzzleIndex);
		}

		int targetIndex = snapshot.getCurrentPuzzleIndex();

		if (targetIndex < 0 || targetIndex >= puzzleCount)
		{
			return;
		}

		this->currentPuzzleIndex = targetIndex;

		Model::Puzzle puzzle = this->repository->getPuzzle(targetIndex);
		this->engine->loadFromPuzzle(puzzle);

		this->restoreBoardFromMemory(targetIndex);
	}

	void GameController::tick()
	{
		if (!this->solvedPuzzles[this->currentPuzzleIndex])
		{
			this->savedSeconds[this->currentPuzzleIndex] += Constants::TIMER_INCREMENT_SECONDS;
		}
	}

	void GameController::resetTimer()
	{
		this->savedSeconds[this->currentPuzzleIndex] = 0;
	}

	int GameController::getElapsedSeconds() const
	{
		return this->savedSeconds[this->currentPuzzleIndex];
	}
}
