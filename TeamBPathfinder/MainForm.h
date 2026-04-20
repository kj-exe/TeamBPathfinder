#pragma once

#pragma managed(push, off)
#include "GameEngine.h"
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
			SetupGrid();
			LoadPuzzleA();
		}

	protected:
		~MainForm()
		{
			if (components)
				delete components;

			if (gameEngine)
				delete gameEngine;
		}

	private:
		static const int GRID_MARGIN = 20;
		static const int GRID_TOP = 50;

		Label^ labelPuzzle;
		System::ComponentModel::Container^ components;
		GameEngine* gameEngine;
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

		void LoadPuzzleA()
		{
			gameEngine->initBoard();

			gameEngine->setFixed(0, 0, 1);
			gameEngine->setFixed(0, 7, 8);
			gameEngine->setFixed(1, 1, 29);
			gameEngine->setFixed(1, 6, 44);
			gameEngine->setFixed(3, 2, 56);
			gameEngine->setFixed(3, 5, 49);
			gameEngine->setFixed(5, 2, 58);
			gameEngine->setFixed(5, 5, 61);
			gameEngine->setFixed(6, 1, 34);
			gameEngine->setFixed(6, 6, 39);
			gameEngine->setFixed(7, 0, 22);
			gameEngine->setFixed(7, 7, 15);

			RefreshGrid();
		}

		void RefreshGrid()
		{
			for (int row = 0; row < GridPanel::BOARD_SIZE; row++)
				for (int col = 0; col < GridPanel::BOARD_SIZE; col++)
					gridPanel->UpdateCell(row, col, gameEngine->getValue(row, col), gameEngine->isFixed(row, col));
		}

		void HandleCellInput(int row, int col, String^ text)
		{
			if (gameEngine->isFixed(row, col))
				return;

			if (String::IsNullOrEmpty(text))
			{
				gameEngine->removeNumber(row, col);
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

			AttemptMove(row, col, value);
		}

		void AttemptMove(int row, int col, int value)
		{
			gameEngine->removeNumber(row, col);

			if (!gameEngine->placeNumber(row, col, value))
			{
				ShowMessage(
					"Invalid move! Number " + value.ToString() + " is either out of range or already on the board.",
					"Invalid Move"
				);
				gridPanel->ClearCell(row, col);
				return;
			}

			gridPanel->ResetCellColor(row, col);
			CheckForCompletion();
		}

		void CheckForCompletion()
		{
			if (!gameEngine->isBoardComplete())
				return;

			if (gameEngine->isSolutionCorrect())
				ShowMessage("Congratulations! You solved the puzzle!", "Puzzle Complete");
			else
				ShowMessage("The board is full but the solution is not correct. Keep trying!", "Not Quite");
		}

		void ShowMessage(String^ message, String^ title)
		{
			MessageBox::Show(message, title, MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
	};
}
