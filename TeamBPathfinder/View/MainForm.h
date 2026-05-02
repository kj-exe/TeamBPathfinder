#pragma once

#pragma managed(push, off)
#include "../Engine/GameEngine.h"
#include "../Model/PuzzleRepository.h"
#include "../Controller/GameController.h"
#include "../Utils/PathHelper.h"
#pragma managed(pop)

#include "GridPanel.h"


namespace TeamBPathfinder
{
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

			try
			{
				std::string puzzlePath = Utils::PathHelper::getPuzzleFilePath();
				repository = new PuzzleRepository(puzzlePath);

			}
			catch (const std::exception& ex)
			{
				MessageBox::Show(
					gcnew String(ex.what()),
					"Error Loading Puzzles",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);

				throw;
			}

			controller = new GameController(gameEngine, repository);

			SetUpUI();
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
		Button^ debugSolveButton;
		System::ComponentModel::Container^ components;
		GameEngine* gameEngine;
		PuzzleRepository* repository;
		GameController* controller;

		System::Windows::Forms::Button^ resetButton;
		System::Windows::Forms::Button^ submitButton;

		GridPanel^ gridPanel;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->resetButton = (gcnew System::Windows::Forms::Button());
			this->submitButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();

			this->resetButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12,
			                                                       System::Drawing::FontStyle::Regular,
			                                                       System::Drawing::GraphicsUnit::Point,
			                                                       static_cast<System::Byte>(0)));
			this->resetButton->Location = System::Drawing::Point(12, 501);
			this->resetButton->Name = L"resetButton";
			this->resetButton->Size = System::Drawing::Size(100, 40);
			this->resetButton->TabIndex = 0;
			this->resetButton->Text = L"Reset";
			this->resetButton->UseVisualStyleBackColor = true;
			this->resetButton->Click += gcnew System::EventHandler(this, &MainForm::resetButton_Click);

			this->submitButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12,
			                                                        System::Drawing::FontStyle::Regular,
			                                                        System::Drawing::GraphicsUnit::Point,
			                                                        static_cast<System::Byte>(0)));
			this->submitButton->Location = System::Drawing::Point(343, 501);
			this->submitButton->Name = L"submitButton";
			this->submitButton->Size = System::Drawing::Size(100, 40);
			this->submitButton->TabIndex = 1;
			this->submitButton->Text = L"Submit";
			this->submitButton->UseVisualStyleBackColor = true;
			this->submitButton->Click += gcnew System::EventHandler(this, &MainForm::submitButton_Click);

			this->ClientSize = System::Drawing::Size(482, 570);
			this->Controls->Add(this->submitButton);
			this->Controls->Add(this->resetButton);
			this->Name = L"MainForm";
			this->ResumeLayout(false);
		}
#pragma endregion

		void SetUpUI()
		{
			int formWidth = GRID_MARGIN * 2 + GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
			int formHeight = GRID_TOP + GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 80;
			int buttonRowY = GRID_TOP + GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 10;

			this->labelPuzzle = (gcnew Label());
			this->labelPuzzle->AutoSize = true;
			this->labelPuzzle->Font = (gcnew Drawing::Font(L"Segoe UI", 14, FontStyle::Bold));
			this->labelPuzzle->Location = Point(GRID_MARGIN, 12);
			this->labelPuzzle->Text = L"";

			this->debugSolveButton = (gcnew Button());
			this->debugSolveButton->Font = (gcnew Drawing::Font(
				L"Microsoft Sans Serif",
				12,
				FontStyle::Regular
			));
			this->debugSolveButton->Text = L"Debug: Solve";
			this->debugSolveButton->Size = Drawing::Size(120, 40);
			this->debugSolveButton->Location = Point(GRID_MARGIN + 240, buttonRowY);
			this->debugSolveButton->Click += gcnew EventHandler(this, &MainForm::OnDebugSolveClicked);
			this->Controls->Add(this->debugSolveButton);

			this->ClientSize = Drawing::Size(formWidth, formHeight);
			this->Controls->Add(this->labelPuzzle);
			this->Text = L"Pathfinder 64 by Andrews & Miranda";

			this->resetButton->Location = Point(GRID_MARGIN, buttonRowY);
			this->submitButton->Location = Point(GRID_MARGIN + 120, buttonRowY);
		}

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

		void AdvanceToNextPuzzle()
		{
			int nextIndex = controller->getCurrentPuzzleNumber();
			if (nextIndex < controller->getPuzzleCount())
			{
				controller->startPuzzle(nextIndex);
				RefreshGrid();
				UpdatePuzzleLabel();
			}
			else
			{
				ShowMessage("You've completed all puzzles! Great job!", "Game Complete");
			}
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
			}
		}

		void ShowMessage(String^ message, String^ title)
		{
			MessageBox::Show(message, title, MessageBoxButtons::OK, MessageBoxIcon::Information);
		}

		System::Void resetButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			this->controller->resetCurrentPuzzle();
			this->RefreshGrid();
		}

		System::Void submitButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			MoveResult result = this->controller->submitPuzzle();

			if (result == MoveResult::PuzzleSolved)
			{
				ShowMessage("Congratulations! You solved the puzzle!", "Puzzle Complete");
				AdvanceToNextPuzzle();
			}
			else if (result == MoveResult::PuzzleIncorrect)
			{
				ShowMessage("The board is full but the solution is not correct. Keep trying!", "Not Quite");
			}
			else
			{
				ShowMessage("Please fill in the whole board before submitting.", "Incomplete Puzzle");
			}
		}

		System::Void OnDebugSolveClicked(System::Object^ sender, System::EventArgs^ e)
		{
			this->controller->solveCurrentPuzzle();
			this->RefreshGrid();
		}
	};
}
