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
#include "GamepadInput.h"
#include "BoardCoordinator.h"
#include "ScoreboardForm.h"
#include "NameEntryForm.h"
#include "SettingsForm.h"
#include <msclr/marshal_cppstd.h>

namespace TeamBPathfinder
{
    using namespace Controller;
    using namespace Engine;
    using namespace Model;
    using namespace Persistence;

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::IO;

    /// <summary>
    /// Represents the main game window for Pathfinder 64.
    /// </summary>
    public ref class MainForm : public Form
    {
    public:
        /// <summary>
        /// Initializes the main form, game objects, user interface, saved progress, and timer.
        /// </summary>
        MainForm(void)
        {
            labelPuzzle = nullptr;
            labelTimer = nullptr;
            pausedOverlay = nullptr;
            debugSolveButton = nullptr;
            resetButton = nullptr;
            submitButton = nullptr;
            menuStrip = nullptr;
            pauseMenuItem = nullptr;
            gameTimer = nullptr;
            components = nullptr;

            gridPanel = nullptr;
            gamepad = nullptr;

            gameEngine = nullptr;
            repository = nullptr;
            controller = nullptr;
            scoreboard = nullptr;
            userSettings = nullptr;

            isPaused = false;

            InitializeComponent();

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

            coordinator = gcnew BoardCoordinator(gridPanel, labelPuzzle, labelTimer, controller);

            loadSavedGame();
            coordinator->refreshAllViews();
            gameTimer->Start();

            this->KeyPreview = true;
            this->KeyDown += gcnew KeyEventHandler(this, &MainForm::onFormKeyDown);
        }

    protected:
        /// <summary>
        /// Saves the current game and stops the timer when the form is closing.
        /// </summary>
        /// <param name="e">The form closing event arguments.</param>
        virtual void OnFormClosing(FormClosingEventArgs^ e) override
        {
            this->ActiveControl = nullptr;

            if (gameTimer != nullptr)
                gameTimer->Stop();

            if (controller != nullptr)
                saveCurrentGame();

            Form::OnFormClosing(e);
        }

        /// <summary>
        /// Releases resources owned by the main form.
        /// </summary>
        ~MainForm()
        {
            if (components) delete components;
            if (controller) delete controller;
            if (repository) delete repository;
            if (gameEngine) delete gameEngine;
            if (scoreboard) delete scoreboard;
            if (userSettings) delete userSettings;
        }

        /// <summary>
        /// Paints the custom Game Boy-style chrome.
        /// </summary>
        /// <param name="e">The paint event arguments.</param>
        virtual void OnPaint(PaintEventArgs^ e) override
        {
            Form::OnPaint(e);

            if (chrome != nullptr)
                chrome->paint(e->Graphics);
        }

        /// <summary>
        /// Handles mouse input for the on-screen Game Boy controls.
        /// </summary>
        /// <param name="e">The mouse event arguments.</param>
        virtual void OnMouseDown(MouseEventArgs^ e) override
        {
            Form::OnMouseDown(e);

            if (isPaused) return;
            if (chrome == nullptr || gamepad == nullptr) return;
            if (e->Button != System::Windows::Forms::MouseButtons::Left) return;

            switch (chrome->hitTest(e->Location))
            {
            case GameBoyChrome::HitResult::DPadUp:    gamepad->moveSelection(-1, 0); break;
            case GameBoyChrome::HitResult::DPadDown:  gamepad->moveSelection(1, 0); break;
            case GameBoyChrome::HitResult::DPadLeft:  gamepad->moveSelection(0, -1); break;
            case GameBoyChrome::HitResult::DPadRight: gamepad->moveSelection(0, 1); break;
            case GameBoyChrome::HitResult::ButtonA:   gamepad->pressA(); break;
            case GameBoyChrome::HitResult::ButtonB:   gamepad->pressB(); break;
            default: break;
            }
        }

        /// <summary>
        /// Handles arrow key movement before normal command key processing.
        /// </summary>
        /// <param name="msg">The command message.</param>
        /// <param name="keyData">The pressed key data.</param>
        /// <returns>True if the key was handled; otherwise, false.</returns>
        virtual bool ProcessCmdKey(Message% msg, Keys keyData) override
        {
            if (!isPaused && gamepad != nullptr)
            {
                if (keyData == Keys::Up) { gamepad->moveSelection(-1, 0); return true; }
                if (keyData == Keys::Down) { gamepad->moveSelection(1, 0);  return true; }
                if (keyData == Keys::Left) { gamepad->moveSelection(0, -1); return true; }
                if (keyData == Keys::Right) { gamepad->moveSelection(0, 1);  return true; }
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
        GamepadInput^ gamepad;
        BoardCoordinator^ coordinator;

        GameEngine* gameEngine;
        PuzzleRepository* repository;
        GameController* controller;
        Scoreboard* scoreboard;
        UserSettings* userSettings;

        bool isPaused;

#pragma region Windows Form Designer generated code
        /// <summary>
        /// Initializes the base form controls and form styling.
        /// </summary>
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

        /// <summary>
        /// Initializes the game engine.
        /// </summary>
        void initializeGameEngine()
        {
            gameEngine = new GameEngine();
        }

        /// <summary>
        /// Initializes the puzzle repository from the puzzle file.
        /// </summary>
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

        /// <summary>
        /// Initializes the game controller.
        /// </summary>
        void initializeGameController()
        {
            controller = new GameController(gameEngine, repository);
        }

        /// <summary>
        /// Initializes the scoreboard and loads saved scores.
        /// </summary>
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

        /// <summary>
        /// Initializes user settings from the settings file.
        /// </summary>
        void initializeUserSettings()
        {
            userSettings = new UserSettings();
            std::string settingsPath = Utils::PathHelper::getSettingsFilePath();
            SettingsFileHandler::loadSettings(settingsPath, *userSettings);
        }

        /// <summary>
        /// Creates a grid theme from the current user settings.
        /// </summary>
        /// <param name="settings">The user settings used to create the theme.</param>
        /// <returns>The grid theme created from the settings.</returns>
        GridTheme^ createThemeFromSettings(UserSettings* settings)
        {
            GridTheme^ theme = gcnew GridTheme();
            theme->CellColor = Color::FromArgb(settings->getCellColor());
            theme->FixedCellColor = Color::FromArgb(settings->getFixedCellColor());
            theme->NumberColor = Color::FromArgb(settings->getNumberColor());
            theme->ErrorColor = GridPanel::LCD_DARK;
            theme->FixedTextColor = GridPanel::LCD_LIGHTEST;
            theme->ErrorTextColor = GridPanel::LCD_LIGHTEST;
            theme->BorderColor = GridPanel::LCD_INK;
            theme->SelectedBorderColor = GridPanel::LCD_INK;
            return theme;
        }

        // ----- UI construction -----

        /// <summary>
        /// Gets the top position of the puzzle grid.
        /// </summary>
        /// <returns>The y-coordinate where the grid should be placed.</returns>
        int getGridTop()
        {
            return GameBoyChrome::LABEL_ROW_Y + GameBoyChrome::LABEL_ROW_H + 16;
        }

        /// <summary>
        /// Builds the main user interface.
        /// </summary>
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

        /// <summary>
        /// Builds the puzzle label and timer label.
        /// </summary>
        /// <param name="formWidth">The width of the form.</param>
        /// <param name="boardPx">The board size in pixels.</param>
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

        /// <summary>
        /// Builds the pause overlay displayed when the game is paused.
        /// </summary>
        /// <param name="boardPx">The board size in pixels.</param>
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

        /// <summary>
        /// Builds the reset, debug, and submit buttons.
        /// </summary>
        /// <param name="formWidth">The width of the form.</param>
        /// <param name="formHeight">The height of the form.</param>
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

        /// <summary>
        /// Applies the standard style to a control button.
        /// </summary>
        /// <param name="button">The button to style.</param>
        /// <param name="text">The text to display on the button.</param>
        void styleControlButton(Button^ button, String^ text)
        {
            button->Text = text;
            button->Font = gcnew Drawing::Font(L"Consolas", 10, FontStyle::Bold);
            button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            button->FlatAppearance->BorderSize = 1;
            button->FlatAppearance->BorderColor = GameBoyChrome::BUTTON_BORDER;
            button->BackColor = GameBoyChrome::PLASTIC_HI;
            button->ForeColor = GameBoyChrome::INK;
            button->UseVisualStyleBackColor = false;
            button->TabStop = false;
        }

        /// <summary>
        /// Builds the puzzle grid and gamepad input handler.
        /// </summary>
        void buildPuzzleGrid()
        {
            gridPanel = gcnew GridPanel();
            gridPanel->applyTheme(createThemeFromSettings(userSettings));
            gridPanel->Location = Point(GameBoyChrome::GRID_MARGIN, getGridTop());
            gridPanel->CellInputSubmitted += gcnew CellInputHandler(this, &MainForm::handleCellInput);
            this->Controls->Add(gridPanel);

            gamepad = gcnew GamepadInput(gridPanel, controller);
        }

        /// <summary>
        /// Builds the game timer.
        /// </summary>
        void buildGameTimer()
        {
            gameTimer = gcnew System::Windows::Forms::Timer();
            gameTimer->Interval = 1000;
            gameTimer->Tick += gcnew EventHandler(this, &MainForm::onTimerTick);
        }

        /// <summary>
        /// Builds the menu bar.
        /// </summary>
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

        /// <summary>
        /// Builds the Game menu.
        /// </summary>
        /// <returns>The completed Game menu item.</returns>
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

        /// <summary>
        /// Builds the Puzzle menu.
        /// </summary>
        /// <returns>The completed Puzzle menu item.</returns>
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

        /// <summary>
        /// Builds the Settings menu.
        /// </summary>
        /// <returns>The completed Settings menu item.</returns>
        ToolStripMenuItem^ buildSettingsMenu()
        {
            ToolStripMenuItem^ menu = gcnew ToolStripMenuItem("Settings");
            ToolStripMenuItem^ openSettings = gcnew ToolStripMenuItem("Colors...");
            openSettings->Click += gcnew EventHandler(this, &MainForm::onSettingsClicked);
            menu->DropDownItems->Add(openSettings);
            return menu;
        }

        /// <summary>
        /// Builds the Scores menu.
        /// </summary>
        /// <returns>The completed Scores menu item.</returns>
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

        // ----- Persistence -----

        /// <summary>
        /// Loads saved game progress if available.
        /// </summary>
        void loadSavedGame()
        {
            std::string savePath = Utils::PathHelper::getSaveFilePath();
            GameSnapshot snapshot(controller->getPuzzleCount());

            if (GameStateFileHandler::loadGameState(savePath, snapshot))
                controller->loadFromSnapshot(snapshot);
            else
                controller->initializeFirstPuzzle();
        }

        /// <summary>
        /// Saves the current game progress.
        /// </summary>
        void saveCurrentGame()
        {
            std::string savePath = Utils::PathHelper::getSaveFilePath();
            GameSnapshot snapshot = controller->getSnapshot();
            GameStateFileHandler::saveGameState(savePath, snapshot);
        }

        // ----- Gamepad input -----

        /// <summary>
        /// Handles keyboard input for Game Boy-style buttons.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The key event arguments.</param>
        void onFormKeyDown(Object^ sender, KeyEventArgs^ e)
        {
            if (isPaused) return;

            switch (e->KeyCode)
            {
            case Keys::A: gamepad->pressA(); e->Handled = true; break;
            case Keys::B: gamepad->pressB(); e->Handled = true; break;
            default: break;
            }
        }

        // ----- Cell input from clicking -----

        /// <summary>
        /// Handles input submitted from a grid cell.
        /// </summary>
        /// <param name="row">The row index of the edited cell.</param>
        /// <param name="col">The column index of the edited cell.</param>
        /// <param name="text">The text entered in the cell.</param>
        void handleCellInput(int row, int col, String^ text)
        {
            if (isPaused) return;
            if (controller->isCurrentPuzzleSolved()) return;

            if (String::IsNullOrEmpty(text))
            {
                if (controller->clearCell(row, col) == MoveResult::Cleared)
                    gridPanel->resetCellStyle(row, col);
                return;
            }

            int value;
            if (!Int32::TryParse(text, value))
            {
                showInfoMessage("Please enter a valid number (1-64).", "Invalid Input");
                gridPanel->clearCellValue(row, col);
                return;
            }

            handleMoveResult(controller->attemptMove(row, col, value), row, col, value);
        }

        /// <summary>
        /// Handles the result of an attempted move.
        /// </summary>
        /// <param name="result">The move result returned by the controller.</param>
        /// <param name="row">The row index of the attempted move.</param>
        /// <param name="col">The column index of the attempted move.</param>
        /// <param name="attemptedValue">The value attempted by the player.</param>
        void handleMoveResult(MoveResult result, int row, int col, int attemptedValue)
        {
            switch (result)
            {
            case MoveResult::Accepted:
                gridPanel->resetCellStyle(row, col);
                break;
            case MoveResult::InvalidNumber:
                showInfoMessage(
                    "Invalid move! Number " + attemptedValue.ToString() +
                    " is either out of range or already on the board.",
                    "Invalid Move");
                gridPanel->clearCellValue(row, col);
                break;
            case MoveResult::InvalidCell:
                gridPanel->clearCellValue(row, col);
                break;
            default:
                break;
            }
        }

        /// <summary>
        /// Toggles the game between paused and unpaused.
        /// </summary>
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

        /// <summary>
        /// Shows an informational message box.
        /// </summary>
        /// <param name="message">The message to display.</param>
        /// <param name="title">The message box title.</param>
        void showInfoMessage(String^ message, String^ title)
        {
            MessageBox::Show(message, title, MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        /// <summary>
        /// Shows a confirmation dialog for a destructive action.
        /// </summary>
        /// <param name="message">The confirmation message.</param>
        /// <param name="title">The confirmation dialog title.</param>
        /// <returns>True if the user confirms the action; otherwise, false.</returns>
        bool confirmDestructiveAction(String^ message, String^ title)
        {
            return MessageBox::Show(message, title, MessageBoxButtons::YesNo, MessageBoxIcon::Warning)
                == System::Windows::Forms::DialogResult::Yes;
        }

        /// <summary>
        /// Handles each timer tick.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        void onTimerTick(Object^ sender, EventArgs^ e)
        {
            if (controller->isCurrentPuzzleSolved()) return;
            controller->tick();
            coordinator->refreshTimerLabel();
        }

        /// <summary>
        /// Handles the reset button click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        System::Void onResetClicked(Object^ sender, EventArgs^ e)
        {
            this->ActiveControl = nullptr;
            if (isPaused) return;

            controller->resetCurrentPuzzle();
            coordinator->refreshGrid();
            coordinator->refreshTimerLabel();
            if (!controller->isCurrentPuzzleSolved())
                gameTimer->Start();
        }

        /// <summary>
        /// Handles the submit button click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        System::Void onSubmitClicked(Object^ sender, EventArgs^ e)
        {
            this->ActiveControl = nullptr;
            if (isPaused) return;
            if (controller->isCurrentPuzzleSolved()) return;

            MoveResult result = controller->submitPuzzle();

            if (result == MoveResult::PuzzleSolved)
                handlePuzzleSolved();
            else if (result == MoveResult::PuzzleIncorrect)
                showInfoMessage("The board is full but the solution is not correct. Keep trying!", "Not Quite");
            else
                showInfoMessage("Please fill in the whole board before submitting.", "Incomplete Puzzle");
        }

        /// <summary>
        /// Handles the completed puzzle flow.
        /// </summary>
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
                "Congratulations! You solved the puzzle in " + BoardCoordinator::formatElapsedTime(finalSeconds) + "!",
                "Puzzle Complete");

            if (!coordinator->advanceToNextPuzzle())
            {
                showInfoMessage("You've completed all puzzles! Great job!", "Game Complete");
            }

            if (!controller->isCurrentPuzzleSolved())
                gameTimer->Start();
        }

        /// <summary>
        /// Records a high score for the completed puzzle.
        /// </summary>
        /// <param name="finalSeconds">The final completion time in seconds.</param>
        /// <returns>True if the score was recorded; otherwise, false.</returns>
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

        /// <summary>
        /// Handles the debug solve button click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        System::Void onDebugSolveClicked(Object^ sender, EventArgs^ e)
        {
            this->ActiveControl = nullptr;
            if (isPaused) return;
            controller->solveCurrentPuzzle();
            coordinator->refreshGrid();
        }

        /// <summary>
        /// Handles the new game menu item click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
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

            gamepad = gcnew GamepadInput(gridPanel, controller);
            coordinator = gcnew BoardCoordinator(gridPanel, labelPuzzle, labelTimer, controller);

            coordinator->refreshAllViews();
        }

        /// <summary>
        /// Deletes the saved game file if it exists.
        /// </summary>
        void deleteSaveFile()
        {
            std::string savePath = Utils::PathHelper::getSaveFilePath();
            String^ managedPath = gcnew String(savePath.c_str());
            if (File::Exists(managedPath))
                File::Delete(managedPath);
        }

        /// <summary>
        /// Handles the pause menu item click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        System::Void onPauseClicked(Object^ sender, EventArgs^ e)
        {
            togglePause();
        }

        /// <summary>
        /// Handles the exit menu item click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        System::Void onExitClicked(Object^ sender, EventArgs^ e)
        {
            this->Close();
        }

        /// <summary>
        /// Handles selecting a puzzle from the puzzle menu.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        System::Void onSelectPuzzleClicked(Object^ sender, EventArgs^ e)
        {
            if (isPaused) togglePause();

            ToolStripMenuItem^ item = safe_cast<ToolStripMenuItem^>(sender);
            int targetIndex = safe_cast<int>(item->Tag);

            controller->startPuzzle(targetIndex);
            coordinator->refreshAllViews();
        }

        /// <summary>
        /// Handles the view scoreboard menu item click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
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

        /// <summary>
        /// Handles the reset scoreboard menu item click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
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

        /// <summary>
        /// Handles the settings menu item click.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        System::Void onSettingsClicked(Object^ sender, EventArgs^ e)
        {
            SettingsForm^ dialog = gcnew SettingsForm(userSettings);
            dialog->ShowDialog(this);

            std::string settingsPath = Utils::PathHelper::getSettingsFilePath();
            SettingsFileHandler::saveSettings(settingsPath, *userSettings);

            gridPanel->applyTheme(createThemeFromSettings(userSettings));
            coordinator->refreshGrid();
        }
    };
}