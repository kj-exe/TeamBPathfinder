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
    using namespace System::Drawing::Drawing2D;
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

            userSettings = new UserSettings();
            std::string settingsPath = Utils::PathHelper::getSettingsFilePath();
            SettingsFileHandler::loadSettings(settingsPath, *userSettings);

            SetUpUI();
            SetupGrid();
            SetupTimer();
            SetupMenuBar();

            LoadSavedGame();

            RefreshGrid();
            UpdatePuzzleLabel();
            UpdateTimerLabel();
            gameTimer->Start();

            this->KeyPreview = true;
            this->KeyDown += gcnew KeyEventHandler(this, &MainForm::OnFormKeyDown);
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
            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::AntiAlias;

            DrawConsoleChrome(g);
            DrawScreenBezel(g);
            DrawBranding(g);
            DrawSpeakerGrille(g);
            DrawDPadAndButtons(g);
            DrawStartSelect(g);
        }

        virtual void OnMouseDown(MouseEventArgs^ e) override
        {
            Form::OnMouseDown(e);

            if (isPaused) return;
            if (e->Button != ::System::Windows::Forms::MouseButtons::Left) return;

            if (dpadUp.Contains(e->Location)) { MoveSelection(-1, 0); return; }
            if (dpadDown.Contains(e->Location)) { MoveSelection(1, 0); return; }
            if (dpadLeft.Contains(e->Location)) { MoveSelection(0, -1); return; }
            if (dpadRight.Contains(e->Location)) { MoveSelection(0, 1); return; }

            int aDx = e->X - aButtonCenterX;
            int aDy = e->Y - aButtonCenterY;
            if (aDx * aDx + aDy * aDy <= abButtonRadius * abButtonRadius)
            {
                PressAButton();
                return;
            }
            int bDx = e->X - bButtonCenterX;
            int bDy = e->Y - bButtonCenterY;
            if (bDx * bDx + bDy * bDy <= abButtonRadius * abButtonRadius)
            {
                PressBButton();
                return;
            }
        }

    private:
        static initonly Color GB_PLASTIC = Color::FromArgb(196, 190, 183);
        static initonly Color GB_PLASTIC_HI = Color::FromArgb(220, 215, 208);
        static initonly Color GB_PLASTIC_LO = Color::FromArgb(150, 144, 138);
        static initonly Color GB_PLASTIC_SHADE = Color::FromArgb(170, 164, 158);
        static initonly Color GB_BEZEL = Color::FromArgb(48, 56, 80);
        static initonly Color GB_BEZEL_DK = Color::FromArgb(28, 34, 52);
        static initonly Color LCD_LIGHTEST = Color::FromArgb(155, 188, 15);
        static initonly Color LCD_INK = Color::FromArgb(15, 56, 15);
        static initonly Color BTN_AB = Color::FromArgb(146, 38, 86);
        static initonly Color BTN_AB_HI = Color::FromArgb(180, 60, 110);
        static initonly Color POWER_LED = Color::FromArgb(220, 40, 40);

        static const int GRID_MARGIN = 28;
        static const int MENU_HEIGHT = 24;
        static const int BRAND_TOP = MENU_HEIGHT + 14;
        static const int BRAND_HEIGHT = 48;
        static const int LABEL_ROW_Y = MENU_HEIGHT + BRAND_HEIGHT + 18;
        static const int LABEL_ROW_H = 22;
        static const int GRID_TOP = LABEL_ROW_Y + LABEL_ROW_H + 16;
        static const int CONTROLS_GAP = 22;
        static const int CONTROLS_ROW_H = 230;
        static const int BOTTOM_BTN_H = 38;
        static const int BOTTOM_BTN_GAP = 14;

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
        UserSettings* userSettings;

        Drawing::Rectangle dpadUp;
        Drawing::Rectangle dpadDown;
        Drawing::Rectangle dpadLeft;
        Drawing::Rectangle dpadRight;
        int aButtonCenterX;
        int aButtonCenterY;
        int bButtonCenterX;
        int bButtonCenterY;
        int abButtonRadius;

        System::Windows::Forms::Button^ resetButton;
        System::Windows::Forms::Button^ submitButton;
        GridPanel^ gridPanel;

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->resetButton = (gcnew System::Windows::Forms::Button());
            this->submitButton = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();

            this->resetButton->Font = (gcnew System::Drawing::Font(L"Consolas", 11,
                System::Drawing::FontStyle::Bold,
                System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->resetButton->Location = System::Drawing::Point(12, 501);
            this->resetButton->Name = L"resetButton";
            this->resetButton->Size = System::Drawing::Size(100, 40);
            this->resetButton->TabIndex = 0;
            this->resetButton->Text = L"RESET";
            this->resetButton->UseVisualStyleBackColor = true;
            this->resetButton->Click += gcnew System::EventHandler(this, &MainForm::resetButton_Click);

            this->submitButton->Font = (gcnew System::Drawing::Font(L"Consolas", 11,
                System::Drawing::FontStyle::Bold,
                System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->submitButton->Location = System::Drawing::Point(343, 501);
            this->submitButton->Name = L"submitButton";
            this->submitButton->Size = System::Drawing::Size(100, 40);
            this->submitButton->TabIndex = 1;
            this->submitButton->Text = L"SUBMIT";
            this->submitButton->UseVisualStyleBackColor = true;
            this->submitButton->Click += gcnew System::EventHandler(this, &MainForm::submitButton_Click);

            this->ClientSize = System::Drawing::Size(482, 570);
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
            this->BackColor = GB_PLASTIC;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->MaximizeBox = false;
        }
#pragma endregion

        void SetUpUI()
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int formWidth = GRID_MARGIN * 2 + boardPx;
            int formHeight = GRID_TOP + boardPx + CONTROLS_GAP + CONTROLS_ROW_H;
            int controlsRowY = GRID_TOP + boardPx + CONTROLS_GAP;

            this->labelPuzzle = (gcnew Label());
            this->labelPuzzle->AutoSize = false;
            this->labelPuzzle->Font = (gcnew Drawing::Font(L"Consolas", 12, FontStyle::Bold));
            this->labelPuzzle->ForeColor = Color::FromArgb(40, 40, 50);
            this->labelPuzzle->BackColor = Color::Transparent;
            this->labelPuzzle->Size = Drawing::Size(180, LABEL_ROW_H);
            this->labelPuzzle->Location = Point(GRID_MARGIN + 4, LABEL_ROW_Y);
            this->labelPuzzle->TextAlign = ContentAlignment::MiddleLeft;
            this->labelPuzzle->Text = L"";

            this->labelTimer = (gcnew Label());
            this->labelTimer->AutoSize = false;
            this->labelTimer->Font = (gcnew Drawing::Font(L"Consolas", 12, FontStyle::Bold));
            this->labelTimer->ForeColor = Color::FromArgb(40, 40, 50);
            this->labelTimer->BackColor = Color::Transparent;
            this->labelTimer->Size = Drawing::Size(120, LABEL_ROW_H);
            this->labelTimer->Location = Point(formWidth - 120 - GRID_MARGIN - 4, LABEL_ROW_Y);
            this->labelTimer->TextAlign = ContentAlignment::MiddleRight;
            this->labelTimer->Text = L"00:00";

            this->pausedOverlay = (gcnew Label());
            this->pausedOverlay->AutoSize = false;
            this->pausedOverlay->Font = (gcnew Drawing::Font(L"Consolas", 36, FontStyle::Bold));
            this->pausedOverlay->Size = Drawing::Size(boardPx, boardPx);
            this->pausedOverlay->Location = Point(GRID_MARGIN, GRID_TOP);
            this->pausedOverlay->TextAlign = ContentAlignment::MiddleCenter;
            this->pausedOverlay->Text = L"-PAUSED-";
            this->pausedOverlay->BackColor = LCD_LIGHTEST;
            this->pausedOverlay->ForeColor = LCD_INK;
            this->pausedOverlay->BorderStyle = System::Windows::Forms::BorderStyle::None;
            this->pausedOverlay->Visible = false;

            StyleControlButton(this->resetButton, L"RESET");
            StyleControlButton(this->submitButton, L"SUBMIT");

            this->debugSolveButton = (gcnew Button());
            StyleControlButton(this->debugSolveButton, L"DEBUG");
            this->debugSolveButton->Click += gcnew EventHandler(this, &MainForm::OnDebugSolveClicked);
            this->Controls->Add(this->debugSolveButton);

            int btnH = BOTTOM_BTN_H;
            int btnW = 110;
            int totalBtnW = btnW * 3 + BOTTOM_BTN_GAP * 2;
            int btnRowY = formHeight - btnH - 14;
            int btnRowX = (formWidth - totalBtnW) / 2;

            this->resetButton->Size = Drawing::Size(btnW, btnH);
            this->debugSolveButton->Size = Drawing::Size(btnW, btnH);
            this->submitButton->Size = Drawing::Size(btnW, btnH);

            this->resetButton->Location = Point(btnRowX, btnRowY);
            this->debugSolveButton->Location = Point(btnRowX + btnW + BOTTOM_BTN_GAP, btnRowY);
            this->submitButton->Location = Point(btnRowX + (btnW + BOTTOM_BTN_GAP) * 2, btnRowY);

            this->ClientSize = Drawing::Size(formWidth, formHeight);
            this->Controls->Add(this->labelPuzzle);
            this->Controls->Add(this->labelTimer);
            this->Controls->Add(this->pausedOverlay);
            this->Text = L"Pathfinder 64 by Andrews & Miranda";
        }

        void StyleControlButton(Button^ b, String^ text)
        {
            b->Text = text;
            b->Font = gcnew Drawing::Font(L"Consolas", 10, FontStyle::Bold);
            b->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            b->FlatAppearance->BorderSize = 1;
            b->FlatAppearance->BorderColor = Color::FromArgb(80, 80, 90);
            b->BackColor = GB_PLASTIC_HI;
            b->ForeColor = Color::FromArgb(40, 40, 50);
            b->UseVisualStyleBackColor = false;
            b->TabStop = false;
        }

        void SetupGrid()
        {
            gridPanel = gcnew GridPanel();
            gridPanel->ApplySettings(userSettings);
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
            menuStrip->BackColor = GB_PLASTIC_SHADE;
            menuStrip->ForeColor = Color::FromArgb(40, 40, 50);
            menuStrip->Renderer = gcnew ToolStripProfessionalRenderer();
            menuStrip->Font = gcnew Drawing::Font(L"Consolas", 9, FontStyle::Bold);

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

            ToolStripMenuItem^ settingsMenu = gcnew ToolStripMenuItem("Settings");
            ToolStripMenuItem^ openSettings = gcnew ToolStripMenuItem("Colors...");
            openSettings->Click += gcnew EventHandler(this, &MainForm::OnSettingsClicked);
            settingsMenu->DropDownItems->Add(openSettings);

            menuStrip->Items->Add(gameMenu);
            menuStrip->Items->Add(puzzleMenu);
            menuStrip->Items->Add(settingsMenu);
            menuStrip->Items->Add(scoresMenu);

            this->MainMenuStrip = menuStrip;
            this->Controls->Add(menuStrip);
        }

        void OnFormKeyDown(Object^ sender, KeyEventArgs^ e)
        {
            if (isPaused) return;

            switch (e->KeyCode)
            {
            case Keys::A:     PressAButton();       e->Handled = true; break;
            case Keys::B:     PressBButton();       e->Handled = true; break;
            }
        }

    protected:
        virtual bool ProcessCmdKey(Message% msg, Keys keyData) override
        {
            if (!isPaused)
            {
                if (keyData == Keys::Up)    { MoveSelection(-1, 0); return true; }
                if (keyData == Keys::Down)  { MoveSelection(1, 0);  return true; }
                if (keyData == Keys::Left)  { MoveSelection(0, -1); return true; }
                if (keyData == Keys::Right) { MoveSelection(0, 1);  return true; }
            }
            return Form::ProcessCmdKey(msg, keyData);
        }

    private:

        void MoveSelection(int dRow, int dCol)
        {
            int newRow = gridPanel->GetSelectedRow() + dRow;
            int newCol = gridPanel->GetSelectedCol() + dCol;

            if (newRow < 0 || newRow >= GridPanel::BOARD_SIZE) return;
            if (newCol < 0 || newCol >= GridPanel::BOARD_SIZE) return;

            this->ActiveControl = nullptr;
            gridPanel->SetSelectedCell(newRow, newCol);
            gridPanel->FocusSelectedCell();
        }

        void PressAButton()
        {
            if (controller->isCurrentPuzzleSolved()) return;

            int row = gridPanel->GetSelectedRow();
            int col = gridPanel->GetSelectedCol();

            if (controller->isFixed(row, col) || controller->getValue(row, col) > 0)
            {
                if (!FindNextEmptyCell(row, col))
                    return;
                gridPanel->SetSelectedCell(row, col);
            }

            int nextNumber = ComputeNextAvailableNumber();
            if (nextNumber < 1) return;

            MoveResult result = controller->attemptMove(row, col, nextNumber);
            if (result == MoveResult::Accepted)
            {
                gridPanel->UpdateCell(row, col, nextNumber, false);
                gridPanel->ResetCellColor(row, col);
            }
        }

        void PressBButton()
        {
            if (controller->isCurrentPuzzleSolved()) return;

            int row = gridPanel->GetSelectedRow();
            int col = gridPanel->GetSelectedCol();

            if (controller->isFixed(row, col)) return;

            controller->clearCell(row, col);
            gridPanel->UpdateCell(row, col, 0, false);
            gridPanel->ResetCellColor(row, col);
        }

        bool FindNextEmptyCell(int% row, int% col)
        {
            int startIndex = row * GridPanel::BOARD_SIZE + col;
            int total = GridPanel::BOARD_SIZE * GridPanel::BOARD_SIZE;

            for (int i = 0; i < total; i++)
            {
                int idx = (startIndex + i) % total;
                int r = idx / GridPanel::BOARD_SIZE;
                int c = idx % GridPanel::BOARD_SIZE;

                if (!controller->isFixed(r, c) && controller->getValue(r, c) == 0)
                {
                    row = r;
                    col = c;
                    return true;
                }
            }
            return false;
        }

        int ComputeNextAvailableNumber()
        {
            array<bool>^ used = gcnew array<bool>(65);
            for (int r = 0; r < GridPanel::BOARD_SIZE; r++)
                for (int c = 0; c < GridPanel::BOARD_SIZE; c++)
                {
                    int v = controller->getValue(r, c);
                    if (v >= 1 && v <= 64)
                        used[v] = true;
                }
            for (int n = 1; n <= 64; n++)
                if (!used[n])
                    return n;
            return -1;
        }


        void DrawConsoleChrome(Graphics^ g)
        {
            int w = this->ClientSize.Width;
            int h = this->ClientSize.Height;

            Pen^ hi = gcnew Pen(GB_PLASTIC_HI, 2.0f);
            Pen^ lo = gcnew Pen(GB_PLASTIC_LO, 2.0f);

            g->DrawLine(hi, 0, MENU_HEIGHT, w - 1, MENU_HEIGHT);
            g->DrawLine(hi, 0, MENU_HEIGHT, 0, h - 1);
            g->DrawLine(lo, w - 1, MENU_HEIGHT, w - 1, h - 1);
            g->DrawLine(lo, 0, h - 1, w - 1, h - 1);

            int seamY = GRID_TOP + GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + CONTROLS_GAP - 6;
            Pen^ seamLo = gcnew Pen(GB_PLASTIC_LO, 1.0f);
            Pen^ seamHi = gcnew Pen(GB_PLASTIC_HI, 1.0f);
            g->DrawLine(seamLo, 8, seamY, w - 8, seamY);
            g->DrawLine(seamHi, 8, seamY + 1, w - 8, seamY + 1);

            delete hi; delete lo; delete seamLo; delete seamHi;
        }

        void DrawScreenBezel(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int padX = 14, padY = 14;
            Drawing::Rectangle bezel = Drawing::Rectangle(
                GRID_MARGIN - padX,
                GRID_TOP - padY,
                boardPx + padX * 2,
                boardPx + padY * 2);

            SolidBrush^ darkBr = gcnew SolidBrush(GB_BEZEL_DK);
            FillChamferedRect(g, darkBr, bezel, 10, 18);
            Drawing::Rectangle inner = Drawing::Rectangle(bezel.X + 3, bezel.Y + 3, bezel.Width - 6, bezel.Height - 6);
            SolidBrush^ bezBr = gcnew SolidBrush(GB_BEZEL);
            FillChamferedRect(g, bezBr, inner, 8, 15);

            String^ tag = L"- LCD MATRIX -";
            Drawing::Font^ tagFont = gcnew Drawing::Font(L"Consolas", 7, FontStyle::Bold);
            SizeF tagSize = g->MeasureString(tag, tagFont);
            SolidBrush^ tagBr = gcnew SolidBrush(Color::FromArgb(140, 150, 180));
            g->DrawString(
                tag, tagFont, tagBr,
                bezel.X + (bezel.Width - tagSize.Width) / 2,
                bezel.Bottom - tagSize.Height - 2);

            int ledX = GRID_MARGIN - padX + 4;
            int ledY = GRID_TOP + boardPx / 2 - 4;
            SolidBrush^ ledOuter = gcnew SolidBrush(Color::FromArgb(80, 0, 0));
            SolidBrush^ ledInner = gcnew SolidBrush(POWER_LED);
            g->FillEllipse(ledOuter, ledX - 1, ledY - 1, 8, 8);
            g->FillEllipse(ledInner, ledX, ledY, 6, 6);
            Drawing::Font^ pwrFont = gcnew Drawing::Font(L"Consolas", 6, FontStyle::Bold);
            SolidBrush^ pwrBr = gcnew SolidBrush(Color::FromArgb(100, 100, 110));
            g->DrawString(L"PWR", pwrFont, pwrBr, (float)(ledX - 3), (float)(ledY + 8));

            delete darkBr; delete bezBr; delete tagBr; delete tagFont;
            delete ledOuter; delete ledInner; delete pwrFont; delete pwrBr;
        }

        void DrawBranding(Graphics^ g)
        {
            Drawing::Font^ brandFont = gcnew Drawing::Font(
                L"Consolas", 18,
                (FontStyle)(FontStyle::Bold | FontStyle::Italic));
            String^ brand = L"PATHFINDER 64";
            SolidBrush^ brandBr = gcnew SolidBrush(Color::FromArgb(40, 40, 55));
            SolidBrush^ brandShadow = gcnew SolidBrush(Color::FromArgb(60, 0, 0, 0));

            int x = GRID_MARGIN + 6;
            int y = BRAND_TOP;
            g->DrawString(brand, brandFont, brandShadow, (float)(x + 1), (float)(y + 1));
            g->DrawString(brand, brandFont, brandBr, (float)x, (float)y);

            Drawing::Font^ subFont = gcnew Drawing::Font(L"Consolas", 7, FontStyle::Bold);
            SolidBrush^ subBr = gcnew SolidBrush(Color::FromArgb(110, 110, 120));
            g->DrawString(L"PUZZLE SYSTEM \u00B7 8-BIT EDITION", subFont, subBr,
                (float)x, (float)(y + 24));

            Pen^ chev = gcnew Pen(Color::FromArgb(40, 40, 55), 2.0f);
            int cx = x - 14, cy = y + 11;
            g->DrawLine(chev, cx, cy - 5, cx + 5, cy);
            g->DrawLine(chev, cx + 5, cy, cx, cy + 5);

            delete brandFont; delete brandBr; delete brandShadow;
            delete subFont;   delete subBr;   delete chev;
        }

        void DrawSpeakerGrille(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int controlsRowY = GRID_TOP + boardPx + CONTROLS_GAP;
            int baseX = this->ClientSize.Width - GRID_MARGIN - 70;
            int baseY = controlsRowY + CONTROLS_ROW_H - BOTTOM_BTN_H - 36;

            Pen^ slitDk = gcnew Pen(GB_PLASTIC_LO, 2.0f);
            Pen^ slitHi = gcnew Pen(GB_PLASTIC_HI, 1.0f);
            for (int i = 0; i < 6; i++)
            {
                int sx = baseX + i * 8;
                g->DrawLine(slitDk, sx, baseY, sx + 12, baseY - 12);
                g->DrawLine(slitHi, sx + 1, baseY + 1, sx + 13, baseY - 11);
            }
            delete slitDk; delete slitHi;
        }

        void DrawDPadAndButtons(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int controlsRowY = GRID_TOP + boardPx + CONTROLS_GAP;

            int arm = 42;
            int armW = 44;

            int dpadCenterX = GRID_MARGIN + 16 + arm + 10;
            int dpadCenterY = controlsRowY + (CONTROLS_ROW_H - BOTTOM_BTN_H - 28) / 2;

            Drawing::Rectangle hArm = Drawing::Rectangle(
                dpadCenterX - arm - 12, dpadCenterY - armW / 2,
                (arm + 12) * 2, armW);
            Drawing::Rectangle vArm = Drawing::Rectangle(
                dpadCenterX - armW / 2, dpadCenterY - arm - 12,
                armW, (arm + 12) * 2);

            dpadUp = Drawing::Rectangle(vArm.X, vArm.Y, vArm.Width, armW + 12);
            dpadDown = Drawing::Rectangle(vArm.X, dpadCenterY, vArm.Width, armW + 12);
            dpadLeft = Drawing::Rectangle(hArm.X, hArm.Y, armW + 12, hArm.Height);
            dpadRight = Drawing::Rectangle(dpadCenterX, hArm.Y, armW + 12, hArm.Height);

            SolidBrush^ pad = gcnew SolidBrush(Color::FromArgb(34, 34, 38));
            SolidBrush^ padHi = gcnew SolidBrush(Color::FromArgb(70, 70, 80));
            SolidBrush^ shade = gcnew SolidBrush(Color::FromArgb(70, 0, 0, 0));
            g->FillRectangle(shade, hArm.X + 3, hArm.Y + 4, hArm.Width, hArm.Height);
            g->FillRectangle(shade, vArm.X + 3, vArm.Y + 4, vArm.Width, vArm.Height);

            FillRoundedRect(g, pad, hArm, 7);
            FillRoundedRect(g, pad, vArm, 7);

            Drawing::Rectangle cap = Drawing::Rectangle(dpadCenterX - 14, dpadCenterY - 14, 28, 28);
            g->FillRectangle(padHi, cap);
            Pen^ capPen = gcnew Pen(Color::FromArgb(20, 20, 24), 1.0f);
            g->DrawRectangle(capPen, cap);

            SolidBrush^ tri = gcnew SolidBrush(Color::FromArgb(150, 150, 160));
            int tSize = 7;
            DrawTriangle(g, tri, dpadCenterX, dpadCenterY - arm + 6, tSize, 0);
            DrawTriangle(g, tri, dpadCenterX, dpadCenterY + arm - 6, tSize, 2);
            DrawTriangle(g, tri, dpadCenterX - arm + 6, dpadCenterY, tSize, 3);
            DrawTriangle(g, tri, dpadCenterX + arm - 6, dpadCenterY, tSize, 1);

            delete pad; delete padHi; delete shade; delete tri; delete capPen;

            int btnR = 30;
            int abY = dpadCenterY;

            int aX = this->ClientSize.Width - GRID_MARGIN - btnR - 18;
            int bX = aX - (btnR * 2 + 22);

            aButtonCenterX = aX;
            aButtonCenterY = abY;
            bButtonCenterX = bX;
            bButtonCenterY = abY;
            abButtonRadius = btnR;

            DrawCircleButton(g, bX, abY, btnR, L"DELETE");
            DrawCircleButton(g, aX, abY, btnR, L"PLACE");
        }

        void DrawCircleButton(Graphics^ g, int cx, int cy, int r, String^ label)
        {
            SolidBrush^ shade = gcnew SolidBrush(Color::FromArgb(70, 0, 0, 0));
            g->FillEllipse(shade, cx - r + 2, cy - r + 4, r * 2, r * 2);

            SolidBrush^ body = gcnew SolidBrush(BTN_AB);
            g->FillEllipse(body, cx - r, cy - r, r * 2, r * 2);

            SolidBrush^ hi = gcnew SolidBrush(Color::FromArgb(140, 230, 230, 240));
            g->FillEllipse(hi, cx - r + 4, cy - r + 4, (int)(r * 1.0), (int)(r * 0.6));

            Pen^ rim = gcnew Pen(Color::FromArgb(80, 18, 50), 1.5f);
            g->DrawEllipse(rim, cx - r, cy - r, r * 2, r * 2);

            Drawing::Font^ lblFont = gcnew Drawing::Font(L"Consolas", 11, FontStyle::Bold);
            SolidBrush^ lblBr = gcnew SolidBrush(Color::FromArgb(40, 40, 50));
            SizeF sz = g->MeasureString(label, lblFont);
            g->DrawString(label, lblFont, lblBr,
                (float)(cx - sz.Width / 2),
                (float)(cy + r + 2));

            delete shade; delete body; delete hi; delete rim;
            delete lblFont; delete lblBr;
        }

        void DrawStartSelect(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int controlsRowY = GRID_TOP + boardPx + CONTROLS_GAP;
            int y = controlsRowY + CONTROLS_ROW_H - BOTTOM_BTN_H - 26;
            int cx = this->ClientSize.Width / 2;

            DrawPill(g, cx - 40, y, 32, 9, L"SELECT");
            DrawPill(g, cx + 8, y, 32, 9, L"START");
        }

        void DrawPill(Graphics^ g, int x, int y, int w, int h, String^ label)
        {
            Drawing::Rectangle r = Drawing::Rectangle(x, y, w, h);
            SolidBrush^ shade = gcnew SolidBrush(Color::FromArgb(80, 0, 0, 0));
            FillRoundedRect(g, shade, Drawing::Rectangle(x + 1, y + 2, w, h), h / 2);

            SolidBrush^ body = gcnew SolidBrush(Color::FromArgb(80, 80, 90));
            FillRoundedRect(g, body, r, h / 2);

            Drawing::Font^ font = gcnew Drawing::Font(L"Consolas", 7, FontStyle::Bold);
            SolidBrush^ br = gcnew SolidBrush(Color::FromArgb(60, 60, 70));
            SizeF sz = g->MeasureString(label, font);
            g->DrawString(label, font, br,
                (float)(x + (w - sz.Width) / 2), (float)(y + h + 1));

            delete shade; delete body; delete font; delete br;
        }

        void FillRoundedRect(Graphics^ g, Brush^ brush, Drawing::Rectangle r, int radius)
        {
            if (radius <= 0) { g->FillRectangle(brush, r); return; }
            GraphicsPath^ path = gcnew GraphicsPath();
            int d = radius * 2;
            path->AddArc(r.X, r.Y, d, d, 180, 90);
            path->AddArc(r.Right - d, r.Y, d, d, 270, 90);
            path->AddArc(r.Right - d, r.Bottom - d, d, d, 0, 90);
            path->AddArc(r.X, r.Bottom - d, d, d, 90, 90);
            path->CloseFigure();
            g->FillPath(brush, path);
            delete path;
        }

        void FillChamferedRect(Graphics^ g, Brush^ brush, Drawing::Rectangle r, int chamfer, int curve)
        {
            GraphicsPath^ path = gcnew GraphicsPath();
            path->AddLine(r.X + chamfer, r.Y, r.Right - curve, r.Y);
            path->AddArc(r.Right - curve, r.Y, curve, curve, 270, 90);
            path->AddLine(r.Right, r.Y + curve, r.Right, r.Bottom - curve);
            path->AddArc(r.Right - curve, r.Bottom - curve, curve, curve, 0, 90);
            path->AddLine(r.Right - curve, r.Bottom, r.X + curve, r.Bottom);
            path->AddArc(r.X, r.Bottom - curve, curve, curve, 90, 90);
            path->AddLine(r.X, r.Bottom - curve, r.X, r.Y + chamfer);
            path->AddLine(r.X, r.Y + chamfer, r.X + chamfer, r.Y);
            path->CloseFigure();
            g->FillPath(brush, path);
            delete path;
        }

        void DrawTriangle(Graphics^ g, Brush^ br, int cx, int cy, int size, int dir)
        {
            array<Point>^ pts = gcnew array<Point>(3);
            switch (dir)
            {
            case 0: pts[0] = Point(cx, cy - size); pts[1] = Point(cx - size, cy + size); pts[2] = Point(cx + size, cy + size); break;
            case 1: pts[0] = Point(cx + size, cy); pts[1] = Point(cx - size, cy - size); pts[2] = Point(cx - size, cy + size); break;
            case 2: pts[0] = Point(cx, cy + size); pts[1] = Point(cx - size, cy - size); pts[2] = Point(cx + size, cy - size); break;
            case 3: pts[0] = Point(cx - size, cy); pts[1] = Point(cx + size, cy - size); pts[2] = Point(cx + size, cy + size); break;
            }
            g->FillPolygon(br, pts);
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
            labelPuzzle->Text = "PUZZLE " + controller->getCurrentPuzzleNumber().ToString();
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
            if (isPaused) return;
            if (controller->isCurrentPuzzleSolved()) return;

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
                    "Invalid Move");
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
            this->ActiveControl = nullptr;
            if (isPaused) return;
            this->controller->resetCurrentPuzzle();
            this->RefreshGrid();
            this->UpdateTimerLabel();
            if (!controller->isCurrentPuzzleSolved())
                gameTimer->Start();
        }

        System::Void submitButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            this->ActiveControl = nullptr;
            if (isPaused) return;

            MoveResult result = this->controller->submitPuzzle();

            if (result == MoveResult::PuzzleSolved)
            {
                gameTimer->Stop();
                int time = controller->getElapsedSeconds();

                if (scoreboard->isHighScore(time))
                {
                    NameEntryForm^ nameForm = gcnew NameEntryForm();
                    if (nameForm->ShowDialog() != System::Windows::Forms::DialogResult::OK)
                        return;

                    String^ name = nameForm->GetPlayerName();
                    if (String::IsNullOrWhiteSpace(name))
                        name = "Player";

                    std::string playerName = msclr::interop::marshal_as<std::string>(name);

                    ScoreEntry entry(
                        playerName,
                        controller->getCurrentPuzzleNumber(),
                        time);

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
                    "Puzzle Complete");
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
            this->ActiveControl = nullptr;
            if (isPaused) return;
            this->controller->solveCurrentPuzzle();
            this->RefreshGrid();
        }

        System::Void OnNewGameClicked(Object^ sender, EventArgs^ e)
        {
            System::Windows::Forms::DialogResult result = MessageBox::Show(
                "Start a new game? This will erase all current progress.",
                "Confirm New Game",
                MessageBoxButtons::YesNo,
                MessageBoxIcon::Warning);

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

        System::Void OnPauseClicked(Object^ sender, EventArgs^ e) { TogglePause(); }

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

        System::Void OnExitClicked(Object^ sender, EventArgs^ e) { this->Close(); }

        System::Void OnSelectPuzzleClicked(Object^ sender, EventArgs^ e)
        {
            if (isPaused) TogglePause();
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
                MessageBoxIcon::Warning);

            if (result != System::Windows::Forms::DialogResult::Yes)
                return;

            scoreboard->clear();
            std::string scorePath = Utils::PathHelper::getScoreboardFilePath();
            ScoreboardFileHandler::saveScoreboard(scorePath, *scoreboard);
            ShowMessage("Scoreboard has been reset.", "Reset Complete");
        }

        System::Void OnSettingsClicked(Object^ sender, EventArgs^ e)
        {
            SettingsForm^ dialog = gcnew SettingsForm(userSettings);
            dialog->ShowDialog(this);
            std::string settingsPath = Utils::PathHelper::getSettingsFilePath();
            SettingsFileHandler::saveSettings(settingsPath, *userSettings);
            gridPanel->ApplySettings(userSettings);
            RefreshGrid();
        }
    };
}
