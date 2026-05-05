#pragma once

#pragma managed(push, off)
#include "../Controller/GameController.h"
#pragma managed(pop)

#include "GridPanel.h"

namespace TeamBPathfinder {

    using namespace System;
    using namespace System::Windows::Forms;

    public ref class BoardCoordinator {
    private:
        GridPanel^ gridPanel;
        Label^ labelPuzzle;
        Label^ labelTimer;
        Controller::GameController* controller;

    public:
        BoardCoordinator(GridPanel^ grid, Label^ puzzleLbl, Label^ timerLbl, Controller::GameController* ctrl)
        {
            gridPanel = grid;
            labelPuzzle = puzzleLbl;
            labelTimer = timerLbl;
            controller = ctrl;
        }

        static String^ formatElapsedTime(int totalSeconds)
        {
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            return String::Format("{0:00}:{1:00}", minutes, seconds);
        }

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

        bool advanceToNextPuzzle()
        {
            int nextIndex = controller->getCurrentPuzzleNumber();
            if (nextIndex < controller->getPuzzleCount())
            {
                controller->startPuzzle(nextIndex);
                refreshAllViews();
                return true;
            }
            return false;
        }
    };
}