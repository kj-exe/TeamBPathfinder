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
#include "../Model/UserSettings.h"
#include "../Persistence/SettingsFileHandler.h"
#pragma managed(pop)

#include "GridPanel.h"
#include "GameBoyChrome.h"
#include "ScoreboardForm.h"
#include "NameEntryForm.h"
#include "SettingsForm.h"
#include <msclr/marshal_cppstd.h>

using namespace Controller;
using namespace Engine;
using namespace Model;
using namespace Persistence;

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
            isPaused = false;

            initializeGameEngine();
            initializePuzzleRepository();
            initializeGameController();
            initializeScoreboard();
            initializeUserSettings();

            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int formWidth = GameBoyChrome::GRID_MARGIN * 2 + boardPx;
            int formHeight = getGridTop() + boardPx + GameBoyChrome::CONTROLS_GAP + GameBoyChrome::CONTROLS_ROW_H;
            chrome = gcnew GameBoyChrome(formWidth, formHeight, getGridTop());

            buildUserInterface();
            buildPuzzleGrid();
            buildGameTimer();
            buildMenuBar();

            loadSavedGame();
            refreshAllViews();
            gameTimer->Start();

            this->KeyPreview = true;
            this->KeyDown += gcnew KeyEventHandler(this, &MainForm::onFormKeyDown);
        }

    protected:
        virtual void OnFormClosing(FormClosingEventArgs^ e) override
        {
            this->ActiveControl = nullptr;
            gameTimer->Stop();
            if (controller != nullptr)
                saveCurrentGame();
            Form::OnFormClosing(e);
        }

        ~MainForm()
        {
            if (components) delete components;
            if (controller) delete controller;
            if (repository) delete repository;
            if (gameEngine) delete gameEngine;
            if (scoreboard) delete scoreboard;
            if (userSettings) delete userSettings;
        }

        virtual void OnPaint(PaintEventArgs^ e) override
        {
            Form::OnPaint(e);
            chrome->paint(e->Graphics);
        }

        virtual void OnMouseDown(MouseEventArgs^ e) override
        {
            Form::OnMouseDown(e);

            if (isPaused) return;
            if (e->Button != System::Windows::Forms::MouseButtons::Left) return;

            switch (chrome->hitTest(e->Location))
            {
            case GameBoyChrome::HitResult::DPadUp:    moveSelection(-1, 0); break;
            case GameBoyChrome::HitResult::DPadDown:  moveSelection(1, 0); break;
            case GameBoyChrome::HitResult::DPadLeft:  moveSelection(0, -1); break;
            case GameBoyChrome::HitResult::DPadRight: moveSelection(0, 1); break;
            case GameBoyChrome::HitResult::ButtonA:   pressPlaceButton(); break;
            case GameBoyChrome::HitResult::ButtonB:   pressDeleteButton(); break;
            }
        }

        virtual bool ProcessCmdKey(Message% msg, Keys keyData) override
        {
            if (!isPaused)
            {
                if (keyData == Keys::Up) { moveSelection(-1, 0); return true; }
                if (keyData == Keys::Down) { moveSelection(1, 0);  return true; }
                if (keyData == Keys::Left) { moveSelection(0, -1); return true; }
                if (keyData == Keys::Right) { moveSelection(0, 1);  return true; }
            }
            return Form::ProcessCmdKey(msg, keyData);
        }

    private:
        Label^ labelPuzzle;
        Label^ labelTimer;
        Label^ pausedOverlay;
        Button^ debugSolveButton;
        Button^ resetButton;
        Button^ submitButton;
        MenuStrip^ menuStrip;
        ToolStripMenuItem^ pauseMenuItem;
        System::Windows::Forms::Timer^ gameTimer;
        System::ComponentModel::Container^ components;

        GameBoyChrome^ chrome;
        GridPanel^ gridPanel;

        GameEngine* gameEngine;
        PuzzleRepository* repository;
        GameController* controller;
        Scoreboard* scoreboard;
        UserSettings* userSettings;

        bool isPaused;

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->resetButton = gcnew System::Windows::Forms::Button();
            this->submitButton = gcnew System::Windows::Forms::Button();
            this->SuspendLayout();

            this->resetButton->Click += gcnew System::EventHandler(this, &MainForm::onResetClicked);
            this->submitButton->Click += gcnew System::EventHandler(this, &MainForm::onSubmitClicked);

            this->Controls->Add(this->submitButton);
            this->Controls->Add(this->resetButton);
            this->Name = L"MainForm";
            this->ResumeLayout(false);

            this->DoubleBuffered = true;
            this->SetStyle(
                ControlStyles::AllPaintingInWmPaint |
                ControlStyles::OptimizedDoubleBuffer |
                ControlStyles::UserPaint |
                ControlStyles::ResizeRedraw,
                true);
            this->BackColor = GameBoyChrome::PLASTIC;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->MaximizeBox = false;
        }
#pragma endregion

        // ----- Initialization helpers -----

        void initializeGameEngine()
        {
            gameEngine = new GameEngine();
        }

        void initializePuzzleRepository()
        {
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
                    MessageBoxIcon::Error);
                throw;
            }
        }

        void initializeGameController()
        {
            controller = new GameController(gameEngine, repository);
        }

        void initializeScoreboard()
        {
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
                    MessageBoxIcon::Error);
                throw;
            }
        }

        void initializeUserSettings()
        {
            userSettings = new UserSettings();
            std::string settingsPath = Utils::PathHelper::getSettingsFilePath();
            SettingsFileHandler::loadSettings(settingsPath, *userSettings);
        }

        // ----- UI construction -----

        int getGridTop()
        {
            return GameBoyChrome::LABEL_ROW_Y + GameBoyChrome::LABEL_ROW_H + 16;
        }

        void buildUserInterface()
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int formWidth = GameBoyChrome::GRID_MARGIN * 2 + boardPx;
            int formHeight = getGridTop() + boardPx + GameBoyChrome::CONTROLS_GAP + GameBoyChrome::CONTROLS_ROW_H;

            buildLabels(formWidth, boardPx);
            buildPausedOverlay(boardPx);
            buildBottomButtons(formWidth, formHeight);

            this->ClientSize = Drawing::Size(formWidth, formHeight);
            this->Controls->Add(this->labelPuzzle);
            this->Controls->Add(this->labelTimer);
            this->Controls->Add(this->pausedOverlay);
            this->Text = L"Pathfinder 64 by Andrews & Miranda";
        }

        void buildLabels(int formWidth, int boardPx)
        {
            this->labelPuzzle = gcnew Label();
            this->labelPuzzle->AutoSize = false;
            this->labelPuzzle->Font = gcnew Drawing::Font(L"Consolas", 12, FontStyle::Bold);
            this->labelPuzzle->ForeColor = GameBoyChrome::INK;
            this->labelPuzzle->BackColor = Color::Transparent;
            this->labelPuzzle->Size = Drawing::Size(180, GameBoyChrome::LABEL_ROW_H);
            this->labelPuzzle->Location = Point(GameBoyChrome::GRID_MARGIN + 4, GameBoyChrome::LABEL_ROW_Y);
            this->labelPuzzle->TextAlign = ContentAlignment::MiddleLeft;
            this->labelPuzzle->Text = L"";

            this->labelTimer = gcnew Label();
            this->labelTimer->AutoSize = false;
            this->labelTimer->Font = gcnew Drawing::Font(L"Consolas", 12, FontStyle::Bold);
            this->labelTimer->ForeColor = GameBoyChrome::INK;
            this->labelTimer->BackColor = Color::Transparent;
            this->labelTimer->Size = Drawing::Size(120, GameBoyChrome::LABEL_ROW_H);
            this->labelTimer->Location = Point(formWidth - 120 - GameBoyChrome::GRID_MARGIN - 4, GameBoyChrome::LABEL_ROW_Y);
            this->labelTimer->TextAlign = ContentAlignment::MiddleRight;
            this->labelTimer->Text = L"00:00";
        }

        void buildPausedOverlay(int boardPx)
        {
            this->pausedOverlay = gcnew Label();
            this->pausedOverlay->AutoSize = false;
            this->pausedOverlay->Font = gcnew Drawing::Font(L"Consolas", 36, FontStyle::Bold);
            this->pausedOverlay->Size = Drawing::Size(boardPx, boardPx);
            this->pausedOverlay->Location = Point(GameBoyChrome::GRID_MARGIN, getGridTop());
            this->pausedOverlay->TextAlign = ContentAlignment::MiddleCenter;
            this->pausedOverlay->Text = L"-PAUSED-";
            this->pausedOverlay->BackColor = GridPanel::LCD_LIGHTEST;
            this->pausedOverlay->ForeColor = GridPanel::LCD_INK;
            this->pausedOverlay->BorderStyle = System::Windows::Forms::BorderStyle::None;
            this->pausedOverlay->Visible = false;
        }

        void buildBottomButtons(int formWidth, int formHeight)
        {
            styleControlButton(this->resetButton, L"RESET");
            styleControlButton(this->submitButton, L"SUBMIT");

            this->debugSolveButton = gcnew Button();
            styleControlButton(this->debugSolveButton, L"DEBUG");
            this->debugSolveButton->Click += gcnew EventHandler(this, &MainForm::onDebugSolveClicked);
            this->Controls->Add(this->debugSolveButton);

            const int btnH = GameBoyChrome::BOTTOM_BTN_H;
            const int btnW = 110;
            const int gap = GameBoyChrome::BOTTOM_BTN_GAP;
            int totalBtnW = btnW * 3 + gap * 2;
            int btnRowY = formHeight - btnH - 14;
            int btnRowX = (formWidth - totalBtnW) / 2;

            this->resetButton->Size = Drawing::Size(btnW, btnH);
            this->debugSolveButton->Size = Drawing::Size(btnW, btnH);
            this->submitButton->Size = Drawing::Size(btnW, btnH);

            this->resetButton->Location = Point(btnRowX, btnRowY);
            this->debugSolveButton->Location = Point(btnRowX + btnW + gap, btnRowY);
            this->submitButton->Location = Point(btnRowX + (btnW + gap) * 2, btnRowY);
        }

        void styleControlButton(Button^ button, String^ text)
        {
            button->Text = text;
            button->Font = gcnew Drawing::Font(L"Consolas", 10, FontStyle::Bold);
            button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            button->FlatAppearance->BorderSize = 1;
            button->FlatAppearance->BorderColor = Color::FromArgb(80, 80, 90);
            button->BackColor = GameBoyChrome::PLASTIC_HI;
            button->ForeColor = GameBoyChrome::INK;
            button->UseVisualStyleBackColor = false;
            button->TabStop = false;
        }

        void buildPuzzleGrid()
        {
            gridPanel = gcnew GridPanel();
            gridPanel->ApplySettings(userSettings);
            gridPanel->Location = Point(GameBoyChrome::GRID_MARGIN, getGridTop());
            gridPanel->OnCellInput += gcnew CellInputHandler(this, &MainForm::handleCellInput);
            this->Controls->Add(gridPanel);
        }

        void buildGameTimer()
        {
            gameTimer = gcnew System::Windows::Forms::Timer();
            gameTimer->Interval = 1000;
            gameTimer->Tick += gcnew EventHandler(this, &MainForm::onTimerTick);
        }

        void buildMenuBar()
        {
            menuStrip = gcnew MenuStrip();
            menuStrip->BackColor = GameBoyChrome::PLASTIC_SHADE;
            menuStrip->ForeColor = GameBoyChrome::INK;
            menuStrip->Renderer = gcnew ToolStripProfessionalRenderer();
            menuStrip->Font = gcnew Drawing::Font(L"Consolas", 9, FontStyle::Bold);

            menuStrip->Items->Add(buildGameMenu());
            menuStrip->Items->Add(buildPuzzleMenu());
            menuStrip->Items->Add(buildSettingsMenu());
            menuStrip->Items->Add(buildScoresMenu());

            this->MainMenuStrip = menuStrip;
            this->Controls->Add(menuStrip);
        }

        ToolStripMenuItem^ buildGameMenu()
        {
            ToolStripMenuItem^ menu = gcnew ToolStripMenuItem("Game");

            ToolStripMenuItem^ newGame = gcnew ToolStripMenuItem("New Game");
            newGame->Click += gcnew EventHandler(this, &MainForm::onNewGameClicked);
            menu->DropDownItems->Add(newGame);

            pauseMenuItem = gcnew ToolStripMenuItem("Pause");
            pauseMenuItem->Click += gcnew EventHandler(this, &MainForm::onPauseClicked);
            menu->DropDownItems->Add(pauseMenuItem);

            menu->DropDownItems->Add(gcnew ToolStripSeparator());

            ToolStripMenuItem^ exit = gcnew ToolStripMenuItem("Exit");
            exit->Click += gcnew EventHandler(this, &MainForm::onExitClicked);
            menu->DropDownItems->Add(exit);

            return menu;
        }

        ToolStripMenuItem^ buildPuzzleMenu()
        {
            ToolStripMenuItem^ menu = gcnew ToolStripMenuItem("Puzzle");
            for (int i = 1; i <= controller->getPuzzleCount(); i++)
            {
                ToolStripMenuItem^ item = gcnew ToolStripMenuItem("Puzzle " + i.ToString());
                item->Tag = i - 1;
                item->Click += gcnew EventHandler(this, &MainForm::onSelectPuzzleClicked);
                menu->DropDownItems->Add(item);
            }
            return menu;
        }

        ToolStripMenuItem^ buildSettingsMenu()
        {
            ToolStripMenuItem^ menu = gcnew ToolStripMenuItem("Settings");
            ToolStripMenuItem^ openSettings = gcnew ToolStripMenuItem("Colors...");
            openSettings->Click += gcnew EventHandler(this, &MainForm::onSettingsClicked);
            menu->DropDownItems->Add(openSettings);
            return menu;
        }

        ToolStripMenuItem^ buildScoresMenu()
        {
            ToolStripMenuItem^ menu = gcnew ToolStripMenuItem("Scores");

            ToolStripMenuItem^ viewScores = gcnew ToolStripMenuItem("View Scoreboard");
            viewScores->Click += gcnew EventHandler(this, &MainForm::onViewScoresClicked);
            menu->DropDownItems->Add(viewScores);

            ToolStripMenuItem^ resetScores = gcnew ToolStripMenuItem("Reset Scoreboard");
            resetScores->Click += gcnew EventHandler(this, &MainForm::onResetScoresClicked);
            menu->DropDownItems->Add(resetScores);

            return menu;
        }

        // ----- View synchronization -----

        void refreshAllViews()
        {
            refreshGrid();
            refreshPuzzleLabel();
            refreshTimerLabel();
        }

        void refreshGrid()
        {
            for (int row = 0; row < GridPanel::BOARD_SIZE; row++)
                for (int col = 0; col < GridPanel::BOARD_SIZE; col++)
                    gridPanel->UpdateCell(row, col, controller->getValue(row, col), controller->isFixed(row, col));
        }

        void refreshPuzzleLabel()
        {
            labelPuzzle->Text = "PUZZLE " + controller->getCurrentPuzzleNumber().ToString();
        }

        void refreshTimerLabel()
        {
            labelTimer->Text = formatElapsedTime(controller->getElapsedSeconds());
        }

        String^ formatElapsedTime(int totalSeconds)
        {
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            return String::Format("{0:00}:{1:00}", minutes, seconds);
        }

        void advanceToNextPuzzle()
        {
            int nextIndex = controller->getCurrentPuzzleNumber();
            if (nextIndex < controller->getPuzzleCount())
            {
                controller->startPuzzle(nextIndex);
                refreshAllViews();
            }
            else
            {
                showInfoMessage("You've completed all puzzles! Great job!", "Game Complete");
            }
        }

        // ----- Persistence -----

        void loadSavedGame()
        {
            std::string savePath = Utils::PathHelper::getSaveFilePath();
            GameSnapshot snapshot(controller->getPuzzleCount());

            if (GameStateFileHandler::loadGameState(savePath, snapshot))
                controller->loadFromSnapshot(snapshot);
            else
                controller->initializeFirstPuzzle();
        }

        void saveCurrentGame()
        {
            std::string savePath = Utils::PathHelper::getSaveFilePath();
            GameSnapshot snapshot = controller->getSnapshot();
            GameStateFileHandler::saveGameState(savePath, snapshot);
        }

        // ----- Gamepad input -----

        void onFormKeyDown(Object^ sender, KeyEventArgs^ e)
        {
            if (isPaused) return;

            switch (e->KeyCode)
            {
            case Keys::A: pressPlaceButton(); e->Handled = true; break;
            case Keys::B: pressDeleteButton(); e->Handled = true; break;
            }
        }

        void moveSelection(int rowDelta, int colDelta)
        {
            int newRow = gridPanel->GetSelectedRow() + rowDelta;
            int newCol = gridPanel->GetSelectedCol() + colDelta;

            if (newRow < 0 || newRow >= GridPanel::BOARD_SIZE) return;
            if (newCol < 0 || newCol >= GridPanel::BOARD_SIZE) return;

            this->ActiveControl = nullptr;
            gridPanel->SetSelectedCell(newRow, newCol);
            gridPanel->FocusSelectedCell();
        }

        void pressPlaceButton()
        {
            if (controller->isCurrentPuzzleSolved()) return;

            int row = gridPanel->GetSelectedRow();
            int col = gridPanel->GetSelectedCol();

            if (controller->isFixed(row, col) || controller->getValue(row, col) > 0)
            {
                if (!findNextEmptyCell(row, col))
                    return;
                gridPanel->SetSelectedCell(row, col);
            }

            int nextNumber = computeNextAvailableNumber();
            if (nextNumber < 1) return;

            MoveResult result = controller->attemptMove(row, col, nextNumber);
            if (result == MoveResult::Accepted)
            {
                gridPanel->UpdateCell(row, col, nextNumber, false);
                gridPanel->ResetCellColor(row, col);
            }
        }

        void pressDeleteButton()
        {
            if (controller->isCurrentPuzzleSolved()) return;

            int row = gridPanel->GetSelectedRow();
            int col = gridPanel->GetSelectedCol();

            if (controller->isFixed(row, col)) return;

            controller->clearCell(row, col);
            gridPanel->UpdateCell(row, col, 0, false);
            gridPanel->ResetCellColor(row, col);
        }

        bool findNextEmptyCell(int% row, int% col)
        {
            int startIndex = row * GridPanel::BOARD_SIZE + col;
            int total = GridPanel::BOARD_SIZE * GridPanel::BOARD_SIZE;

            for (int i = 0; i < total; i++)
            {
                int idx = (startIndex + i) % total;
                int candidateRow = idx / GridPanel::BOARD_SIZE;
                int candidateCol = idx % GridPanel::BOARD_SIZE;

                if (!controller->isFixed(candidateRow, candidateCol) &&
                    controller->getValue(candidateRow, candidateCol) == 0)
                {
                    row = candidateRow;
                    col = candidateCol;
                    return true;
                }
            }
            return false;
        }

        int computeNextAvailableNumber()
        {
            const int boardCellCount = GridPanel::BOARD_SIZE * GridPanel::BOARD_SIZE;
            array<bool>^ used = gcnew array<bool>(boardCellCount + 1);

            for (int row = 0; row < GridPanel::BOARD_SIZE; row++)
                for (int col = 0; col < GridPanel::BOARD_SIZE; col++)
                {
                    int value = controller->getValue(row, col);
                    if (value >= 1 && value <= boardCellCount)
                        used[value] = true;
                }

            for (int n = 1; n <= boardCellCount; n++)
                if (!used[n])
                    return n;
            return -1;
        }

        // ----- Cell input from clicking -----

        void handleCellInput(int row, int col, String^ text)
        {
            if (isPaused) return;
            if (controller->isCurrentPuzzleSolved()) return;

            if (String::IsNullOrEmpty(text))
            {
                if (controller->clearCell(row, col) == MoveResult::Cleared)
                    gridPanel->ResetCellColor(row, col);
                return;
            }

            int value;
            if (!Int32::TryParse(text, value))
            {
                showInfoMessage("Please enter a valid number (1-64).", "Invalid Input");
                gridPanel->ClearCell(row, col);
                return;
            }

            handleMoveResult(controller->attemptMove(row, col, value), row, col, value);
        }

        void handleMoveResult(MoveResult result, int row, int col, int attemptedValue)
        {
            switch (result)
            {
            case MoveResult::Accepted:
                gridPanel->ResetCellColor(row, col);
                break;
            case MoveResult::InvalidNumber:
                showInfoMessage(
                    "Invalid move! Number " + attemptedValue.ToString() +
                    " is either out of range or already on the board.",
                    "Invalid Move");
                gridPanel->ClearCell(row, col);
                break;
            case MoveResult::InvalidCell:
                gridPanel->ClearCell(row, col);
                break;
            }
        }

        // ----- Pause -----

        void togglePause()
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

        // ----- Messages -----

        void showInfoMessage(String^ message, String^ title)
        {
            MessageBox::Show(message, title, MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        bool confirmDestructiveAction(String^ message, String^ title)
        {
            return MessageBox::Show(message, title, MessageBoxButtons::YesNo, MessageBoxIcon::Warning)
                == System::Windows::Forms::DialogResult::Yes;
        }

        // ----- Event handlers -----

        void onTimerTick(Object^ sender, EventArgs^ e)
        {
            if (controller->isCurrentPuzzleSolved()) return;
            controller->tick();
            refreshTimerLabel();
        }

        System::Void onResetClicked(Object^ sender, EventArgs^ e)
        {
            this->ActiveControl = nullptr;
            if (isPaused) return;

            controller->resetCurrentPuzzle();
            refreshGrid();
            refreshTimerLabel();
            if (!controller->isCurrentPuzzleSolved())
                gameTimer->Start();
        }

        System::Void onSubmitClicked(Object^ sender, EventArgs^ e)
        {
            this->ActiveControl = nullptr;
            if (isPaused) return;

            MoveResult result = controller->submitPuzzle();

            if (result == MoveResult::PuzzleSolved)
                handlePuzzleSolved();
            else if (result == MoveResult::PuzzleIncorrect)
                showInfoMessage("The board is full but the solution is not correct. Keep trying!", "Not Quite");
            else
                showInfoMessage("Please fill in the whole board before submitting.", "Incomplete Puzzle");
        }

        void handlePuzzleSolved()
        {
            gameTimer->Stop();
            int finalSeconds = controller->getElapsedSeconds();

            if (scoreboard->isHighScore(finalSeconds))
            {
                if (!recordHighScore(finalSeconds))
                    return;
            }

            showInfoMessage(
                "Congratulations! You solved the puzzle in " + formatElapsedTime(finalSeconds) + "!",
                "Puzzle Complete");

            advanceToNextPuzzle();
            if (!controller->isCurrentPuzzleSolved())
                gameTimer->Start();
        }

        bool recordHighScore(int finalSeconds)
        {
            NameEntryForm^ nameForm = gcnew NameEntryForm();
            if (nameForm->ShowDialog() != System::Windows::Forms::DialogResult::OK)
                return false;

            String^ name = nameForm->GetPlayerName();
            if (String::IsNullOrWhiteSpace(name))
                name = "Player";

            std::string playerName = msclr::interop::marshal_as<std::string>(name);

            ScoreEntry entry(playerName, controller->getCurrentPuzzleNumber(), finalSeconds);
            scoreboard->addScore(entry);

            std::string scorePath = Utils::PathHelper::getScoreboardFilePath();
            ScoreboardFileHandler::saveScoreboard(scorePath, *scoreboard);
            return true;
        }

        System::Void onDebugSolveClicked(Object^ sender, EventArgs^ e)
        {
            this->ActiveControl = nullptr;
            if (isPaused) return;
            controller->solveCurrentPuzzle();
            refreshGrid();
        }

        System::Void onNewGameClicked(Object^ sender, EventArgs^ e)
        {
            if (!confirmDestructiveAction(
                "Start a new game? This will erase all current progress.",
                "Confirm New Game"))
                return;

            if (isPaused)
                togglePause();

            deleteSaveFile();

            delete controller;
            controller = new GameController(gameEngine, repository);
            controller->initializeFirstPuzzle();

            refreshAllViews();
        }

        void deleteSaveFile()
        {
            std::string savePath = Utils::PathHelper::getSaveFilePath();
            String^ managedPath = gcnew String(savePath.c_str());
            if (File::Exists(managedPath))
                File::Delete(managedPath);
        }

        System::Void onPauseClicked(Object^ sender, EventArgs^ e)
        {
            togglePause();
        }

        System::Void onExitClicked(Object^ sender, EventArgs^ e)
        {
            this->Close();
        }

        System::Void onSelectPuzzleClicked(Object^ sender, EventArgs^ e)
        {
            if (isPaused) togglePause();

            ToolStripMenuItem^ item = safe_cast<ToolStripMenuItem^>(sender);
            int targetIndex = safe_cast<int>(item->Tag);

            controller->startPuzzle(targetIndex);
            refreshAllViews();
        }

        System::Void onViewScoresClicked(Object^ sender, EventArgs^ e)
        {
            if (scoreboard->getScores().empty())
            {
                showInfoMessage("No scores yet.", "Scoreboard");
                return;
            }
            ScoreboardForm^ form = gcnew ScoreboardForm(*scoreboard);
            form->ShowDialog();
        }

        System::Void onResetScoresClicked(Object^ sender, EventArgs^ e)
        {
            if (!confirmDestructiveAction(
                "Are you sure you want to reset the scoreboard?",
                "Confirm Reset"))
                return;

            scoreboard->clear();
            std::string scorePath = Utils::PathHelper::getScoreboardFilePath();
            ScoreboardFileHandler::saveScoreboard(scorePath, *scoreboard);
            showInfoMessage("Scoreboard has been reset.", "Reset Complete");
        }

        System::Void onSettingsClicked(Object^ sender, EventArgs^ e)
        {
            SettingsForm^ dialog = gcnew SettingsForm(userSettings);
            dialog->ShowDialog(this);

            std::string settingsPath = Utils::PathHelper::getSettingsFilePath();
            SettingsFileHandler::saveSettings(settingsPath, *userSettings);

            gridPanel->ApplySettings(userSettings);
            refreshGrid();
        }
    };
}