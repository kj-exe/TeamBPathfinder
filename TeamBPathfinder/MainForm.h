#pragma once

#pragma managed(push, off)
#include "GameEngine.h"
#include "PuzzleRepository.h"
#include "GameController.h"
#pragma managed(pop)

#include "GridPanel.h"

namespace TeamBPathfinder {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class MainForm : public Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			gameEngine = new GameEngine();
			repository = new PuzzleRepository();
			controller = new GameController(gameEngine, repository);

			SetupGrid();
			controller->startPuzzle(0);
			RefreshGrid();
			UpdatePuzzleLabel();
		}

	protected:
		~MainForm()
		{
			if (components)
				delete components;

			if (controller)
				delete controller;

			if (repository)
				delete repository;

			if (gameEngine)
				delete gameEngine;
		}

	private:
		static const int GRID_MARGIN = 20;
		static const int GRID_TOP = 50;

		Label^ labelPuzzle;
		System::ComponentModel::Container^ components;
		GameEngine* gameEngine;
		PuzzleRepository* repository;
		GameController* controller;
		GridPanel^ gridPanel;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->labelPuzzle = (gcnew Label());
			this->SuspendLayout();

			this->labelPuzzle->AutoSize = true;
			this->labelPuzzle->Font = (gcnew Drawing::Font(L"Segoe UI", 14, FontStyle::Bold));
			this->labelPuzzle->Location = Point(GRID_MARGIN, 12);
			this->labelPuzzle->Text = L"Puzzle 1";

			int formWidth = GRID_MARGIN * 2 + GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
			int formHeight = GRID_TOP + GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 40;

			this->AutoScaleDimensions = SizeF(6, 13);
			this->AutoScaleMode = Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = Drawing::Size(formWidth, formHeight);
			this->Controls->Add(this->labelPuzzle);
			this->Name = L"MainForm";
			this->Text = L"Pathfinder 64 by Andrews & Miranda";

			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

		void SetupGrid()
		{
			gridPanel = gcnew GridPanel();
			gridPanel->Location = Point(GRID_MARGIN, GRID_TOP);
			gridPanel->OnCellInput += gcnew CellInputHandler(this, &MainForm::HandleCellInput);
			this->Controls->Add(gridPanel);
		}

		void RefreshGrid()
		{
			for (int row = 0; row < GridPanel::BOARD_SIZE; row++)
				for (int col = 0; col < GridPanel::BOARD_SIZE; col++)
					gridPanel->UpdateCell(row, col, controller->getValue(row, col), controller->isFixed(row, col));
		}

		void UpdatePuzzleLabel()
		{
			labelPuzzle->Text = "Puzzle " + controller->getCurrentPuzzleNumber().ToString();
		}

		void HandleCellInput(int row, int col, String^ text)
		{
			if (String::IsNullOrEmpty(text))
			{
				MoveResult result = controller->clearCell(row, col);
				if (result == MoveResult::Cleared)
					gridPanel->ResetCellColor(row, col);
				return;
			}

			int value;
			if (!Int32::TryParse(text, value))
			{
				ShowMessage("Please enter a valid number (1-64).", "Invalid Input");
				gridPanel->ClearCell(row, col);
				return;
			}

			MoveResult result = controller->attemptMove(row, col, value);

			switch (result)
			{
			case MoveResult::Accepted:
				gridPanel->ResetCellColor(row, col);
				break;

			case MoveResult::InvalidNumber:
				ShowMessage(
					"Invalid move! Number " + value.ToString() + " is either out of range or already on the board.",
					"Invalid Move"
				);
				gridPanel->ClearCell(row, col);
				break;

			case MoveResult::InvalidCell:
				gridPanel->ClearCell(row, col);
				break;

			case MoveResult::PuzzleSolved:
				gridPanel->ResetCellColor(row, col);
				ShowMessage("Congratulations! You solved the puzzle!", "Puzzle Complete");
				break;

			case MoveResult::PuzzleIncorrect:
				gridPanel->ResetCellColor(row, col);
				ShowMessage("The board is full but the solution is not correct. Keep trying!", "Not Quite");
				break;
			}
		}

		void ShowMessage(String^ message, String^ title)
		{
			MessageBox::Show(message, title, MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
	};
}