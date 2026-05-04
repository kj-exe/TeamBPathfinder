#pragma once

#pragma managed(push, off)
#include "../Engine/GameEngine.h"
#include "../Model/PuzzleRepository.h"
#include "../Model/GameSnapshot.h"
#include "../Controller/GameController.h"
#include "../Utils/PathHelper.h"
#include "../Persistence/GameStateFileHandler.h"
#include "../Model/Scoreboard.h"
#include "../Model/ScoreEntry.h"
#include "../Persistence/ScoreboardFileHandler.h"
#pragma managed(pop)

#include "GridPanel.h"
#include "ScoreboardForm.h"
#include "NameEntryForm.h"
#include <msclr/marshal_cppstd.h>




namespace TeamBPathfinder
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::IO;

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
			isPaused = false;
			scoreboard = new Scoreboard();
			try
			{
				std::string scorePath = Utils::PathHelper::getScoreboardFilePath();
				ScoreboardFileHandler::loadScoreboard(scorePath, *scoreboard);
			}
			catch (const std::exception& ex)
			{
				MessageBox::Show(
					gcnew String(ex.what()),
					"Error Loading Scoreboard",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
				throw;
			}

			SetUpUI();
			SetupGrid();
			SetupTimer();
			SetupMenuBar();

			LoadSavedGame();

			RefreshGrid();
			UpdatePuzzleLabel();
			UpdateTimerLabel();
			gameTimer->Start();
		}

	protected:
		virtual void OnFormClosing(FormClosingEventArgs^ e) override
		{
			this->ActiveControl = nullptr;

			gameTimer->Stop();

			if (controller != nullptr)
				SaveCurrentGame();

			Form::OnFormClosing(e);
		}

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

			if (scoreboard)
				delete scoreboard;
		}

	private:
		static const int GRID_MARGIN = 20;
		static const int GRID_TOP = 75;
		static const int MENU_HEIGHT = 24;

		Label^ labelPuzzle;
		Label^ labelTimer;
		Label^ pausedOverlay;
		Button^ debugSolveButton;
		MenuStrip^ menuStrip;
		ToolStripMenuItem^ pauseMenuItem;
		System::Windows::Forms::Timer^ gameTimer;
		System::ComponentModel::Container^ components;
		GameEngine* gameEngine;
		PuzzleRepository* repository;
		GameController* controller;
		bool isPaused;
		Scoreboard* scoreboard;

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
			this->labelPuzzle->Location = Point(GRID_MARGIN, MENU_HEIGHT + 12);
			this->labelPuzzle->Text = L"";

			this->labelTimer = (gcnew Label());
			this->labelTimer->AutoSize = false;
			this->labelTimer->Font = (gcnew Drawing::Font(L"Segoe UI", 14, FontStyle::Bold));
			this->labelTimer->Size = Drawing::Size(100, 28);
			this->labelTimer->Location = Point(formWidth - 100 - GRID_MARGIN, MENU_HEIGHT + 12);
			this->labelTimer->TextAlign = ContentAlignment::MiddleRight;
			this->labelTimer->Text = L"00:00";

			this->pausedOverlay = (gcnew Label());
			this->pausedOverlay->AutoSize = false;
			this->pausedOverlay->Font = (gcnew Drawing::Font(L"Segoe UI", 36, FontStyle::Bold));
			this->pausedOverlay->Size = Drawing::Size(
				GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2,
				GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2
			);
			this->pausedOverlay->Location = Point(GRID_MARGIN, GRID_TOP);
			this->pausedOverlay->TextAlign = ContentAlignment::MiddleCenter;
			this->pausedOverlay->Text = L"PAUSED";
			this->pausedOverlay->BackColor = Color::FromArgb(240, 240, 240);
			this->pausedOverlay->ForeColor = Color::FromArgb(80, 80, 80);
			this->pausedOverlay->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pausedOverlay->Visible = false;

			this->debugSolveButton = (gcnew Button());
			this->debugSolveButton->Font = (gcnew Drawing::Font(L"Microsoft Sans Serif", 12, FontStyle::Regular));
			this->debugSolveButton->Text = L"Debug: Solve";
			this->debugSolveButton->Size = Drawing::Size(120, 40);
			this->debugSolveButton->Location = Point(GRID_MARGIN + 240, buttonRowY);
			this->debugSolveButton->Click += gcnew EventHandler(this, &MainForm::OnDebugSolveClicked);
			this->Controls->Add(this->debugSolveButton);

			this->ClientSize = Drawing::Size(formWidth, formHeight);
			this->Controls->Add(this->labelPuzzle);
			this->Controls->Add(this->labelTimer);
			this->Controls->Add(this->pausedOverlay);
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

		void SetupTimer()
		{
			gameTimer = gcnew System::Windows::Forms::Timer();
			gameTimer->Interval = 1000;
			gameTimer->Tick += gcnew EventHandler(this, &MainForm::OnTimerTick);
		}

		void SetupMenuBar()
		{
			menuStrip = gcnew MenuStrip();

			ToolStripMenuItem^ gameMenu = gcnew ToolStripMenuItem("Game");

			ToolStripMenuItem^ newGame = gcnew ToolStripMenuItem("New Game");
			newGame->Click += gcnew EventHandler(this, &MainForm::OnNewGameClicked);
			gameMenu->DropDownItems->Add(newGame);

			pauseMenuItem = gcnew ToolStripMenuItem("Pause");
			pauseMenuItem->Click += gcnew EventHandler(this, &MainForm::OnPauseClicked);
			gameMenu->DropDownItems->Add(pauseMenuItem);

			gameMenu->DropDownItems->Add(gcnew ToolStripSeparator());

			ToolStripMenuItem^ exit = gcnew ToolStripMenuItem("Exit");
			exit->Click += gcnew EventHandler(this, &MainForm::OnExitClicked);
			gameMenu->DropDownItems->Add(exit);

			ToolStripMenuItem^ puzzleMenu = gcnew ToolStripMenuItem("Puzzle");
			for (int i = 1; i <= controller->getPuzzleCount(); i++)
			{
				ToolStripMenuItem^ item = gcnew ToolStripMenuItem("Puzzle " + i.ToString());
				item->Tag = i - 1;
				item->Click += gcnew EventHandler(this, &MainForm::OnSelectPuzzleClicked);
				puzzleMenu->DropDownItems->Add(item);
			}

			ToolStripMenuItem^ scoresMenu = gcnew ToolStripMenuItem("Scores");

			ToolStripMenuItem^ viewScores = gcnew ToolStripMenuItem("View Scoreboard");
			viewScores->Click += gcnew EventHandler(this, &MainForm::OnViewScoresClicked);
			scoresMenu->DropDownItems->Add(viewScores);

			ToolStripMenuItem^ resetScores = gcnew ToolStripMenuItem("Reset Scoreboard");
			resetScores->Click += gcnew EventHandler(this, &MainForm::OnResetScoresClicked);
			scoresMenu->DropDownItems->Add(resetScores);

			menuStrip->Items->Add(gameMenu);
			menuStrip->Items->Add(puzzleMenu);
			menuStrip->Items->Add(scoresMenu);

			this->MainMenuStrip = menuStrip;
			this->Controls->Add(menuStrip);
		}

		void OnTimerTick(Object^ sender, EventArgs^ e)
		{
			if (controller->isCurrentPuzzleSolved())
				return;

			controller->tick();
			UpdateTimerLabel();
		}

		void UpdateTimerLabel()
		{
			int seconds = controller->getElapsedSeconds();
			int minutes = seconds / 60;
			int remaining = seconds % 60;
			labelTimer->Text = String::Format("{0:00}:{1:00}", minutes, remaining);
		}

		void LoadSavedGame()
		{
			std::string savePath = Utils::PathHelper::getSaveFilePath();
			GameSnapshot snapshot(controller->getPuzzleCount());

			if (GameStateFileHandler::loadGameState(savePath, snapshot))
				controller->loadFromSnapshot(snapshot);
			else
				controller->initializeFirstPuzzle();
		}

		void SaveCurrentGame()
		{
			std::string savePath = Utils::PathHelper::getSaveFilePath();
			GameSnapshot snapshot = controller->getSnapshot();
			GameStateFileHandler::saveGameState(savePath, snapshot);
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
				UpdateTimerLabel();
			}
			else
			{
				ShowMessage("You've completed all puzzles! Great job!", "Game Complete");
			}
		}

		void HandleCellInput(int row, int col, String^ text)
		{
			if (isPaused)
				return;

			if (controller->isCurrentPuzzleSolved())
				return;

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
			if (isPaused)
				return;

			this->controller->resetCurrentPuzzle();
			this->RefreshGrid();
			this->UpdateTimerLabel();
			if (!controller->isCurrentPuzzleSolved())
			{
				gameTimer->Start();
			}
		}

		System::Void submitButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (isPaused)
				return;

			MoveResult result = this->controller->submitPuzzle();

			if (result == MoveResult::PuzzleSolved)
			{
				gameTimer->Stop();
				int time = controller->getElapsedSeconds();

				if (scoreboard->isHighScore(time))
				{
					NameEntryForm^ nameForm = gcnew NameEntryForm();

					if (nameForm->ShowDialog() != System::Windows::Forms::DialogResult::OK)
					{
						return;
					}

					String^ name = nameForm->GetPlayerName();

					if (String::IsNullOrWhiteSpace(name))
					{
						name = "Player";
					}

					std::string playerName = msclr::interop::marshal_as<std::string>(name);

					ScoreEntry entry(
						playerName,
						controller->getCurrentPuzzleNumber(),
						time
					);

					scoreboard->addScore(entry);

					std::string scorePath = Utils::PathHelper::getScoreboardFilePath();
					ScoreboardFileHandler::saveScoreboard(scorePath, *scoreboard);
				}

				int finalSeconds = controller->getElapsedSeconds();
				int finalMinutes = finalSeconds / 60;
				int remainingSeconds = finalSeconds % 60;
				ShowMessage(
					"Congratulations! You solved the puzzle in " +
					finalMinutes.ToString("00") + ":" + remainingSeconds.ToString("00") + "!",
					"Puzzle Complete"
				);
				AdvanceToNextPuzzle();
				if (!controller->isCurrentPuzzleSolved())
					gameTimer->Start();
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
			if (isPaused)
				return;

			this->controller->solveCurrentPuzzle();
			this->RefreshGrid();
		}

		System::Void OnNewGameClicked(Object^ sender, EventArgs^ e)
		{
			System::Windows::Forms::DialogResult result = MessageBox::Show(
				"Start a new game? This will erase all current progress.",
				"Confirm New Game",
				MessageBoxButtons::YesNo,
				MessageBoxIcon::Warning
			);

			if (result != System::Windows::Forms::DialogResult::Yes)
				return;

			if (isPaused)
				TogglePause();

			std::string savePath = Utils::PathHelper::getSaveFilePath();
			String^ managedPath = gcnew String(savePath.c_str());
			if (File::Exists(managedPath))
				File::Delete(managedPath);

			delete controller;
			controller = new GameController(gameEngine, repository);
			controller->initializeFirstPuzzle();

			RefreshGrid();
			UpdatePuzzleLabel();
			UpdateTimerLabel();
		}

		System::Void OnPauseClicked(Object^ sender, EventArgs^ e)
		{
			TogglePause();
		}

		void TogglePause()
		{
			isPaused = !isPaused;

			if (isPaused)
			{
				gameTimer->Stop();
				gridPanel->Visible = false;
				pausedOverlay->Visible = true;
				pauseMenuItem->Text = "Resume";
			}
			else
			{
				gameTimer->Start();
				gridPanel->Visible = true;
				pausedOverlay->Visible = false;
				pauseMenuItem->Text = "Pause";
			}
		}

		System::Void OnExitClicked(Object^ sender, EventArgs^ e)
		{
			this->Close();
		}

		System::Void OnSelectPuzzleClicked(Object^ sender, EventArgs^ e)
		{
			if (isPaused)
				TogglePause();

			ToolStripMenuItem^ item = safe_cast<ToolStripMenuItem^>(sender);
			int targetIndex = safe_cast<int>(item->Tag);

			controller->startPuzzle(targetIndex);
			RefreshGrid();
			UpdatePuzzleLabel();
			UpdateTimerLabel();
		}

		System::Void OnViewScoresClicked(Object^ sender, EventArgs^ e)
		{
			if (scoreboard->getScores().empty())
			{
				ShowMessage("No scores yet.", "Scoreboard");
				return;
			}

			ScoreboardForm^ form = gcnew ScoreboardForm(*scoreboard);
			form->ShowDialog();
		}

		System::Void OnResetScoresClicked(Object^ sender, EventArgs^ e)
		{
			System::Windows::Forms::DialogResult result = MessageBox::Show(
				"Are you sure you want to reset the scoreboard?",
				"Confirm Reset",
				MessageBoxButtons::YesNo,
				MessageBoxIcon::Warning
			);

			if (result != System::Windows::Forms::DialogResult::Yes)
				return;

			scoreboard->clear();

			std::string scorePath = Utils::PathHelper::getScoreboardFilePath();
			ScoreboardFileHandler::saveScoreboard(scorePath, *scoreboard);

			ShowMessage("Scoreboard has been reset.", "Reset Complete");
		}
	};
}