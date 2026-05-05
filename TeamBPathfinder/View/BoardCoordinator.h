#pragma once

#pragma managed(push, off)
#include "../Controller/GameController.h"
#pragma managed(pop)

#include "GridPanel.h"

namespace TeamBPathfinder {

    using namespace System;
    using namespace System::Windows::Forms;

    /// <summary>
    /// Coordinates updates between the game controller and board-related UI elements.
    /// </summary>
    public ref class BoardCoordinator {
    private:
        GridPanel^ gridPanel;
        Label^ labelPuzzle;
        Label^ labelTimer;
        Controller::GameController* controller;

    public:
        /// <summary>
        /// Initializes a new board coordinator.
        /// </summary>
        /// <param name="grid">The grid panel to refresh.</param>
        /// <param name="puzzleLbl">The label that displays the current puzzle number.</param>
        /// <param name="timerLbl">The label that displays the elapsed time.</param>
        /// <param name="ctrl">The controller that provides the current game state.</param>
        BoardCoordinator(GridPanel^ grid, Label^ puzzleLbl, Label^ timerLbl, Controller::GameController* ctrl)
        {
            gridPanel = grid;
            labelPuzzle = puzzleLbl;
            labelTimer = timerLbl;
            controller = ctrl;
        }

        /// <summary>
        /// Formats elapsed seconds as minutes and seconds.
        /// </summary>
        /// <param name="totalSeconds">The elapsed time in seconds.</param>
        /// <returns>The elapsed time formatted as MM:SS.</returns>
        static String^ formatElapsedTime(int totalSeconds)
        {
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            return String::Format("{0:00}:{1:00}", minutes, seconds);
        }

        /// <summary>
        /// Refreshes the grid, puzzle label, and timer label.
        /// </summary>
        void refreshAllViews()
        {
            refreshGrid();
            refreshPuzzleLabel();
            refreshTimerLabel();
        }

        /// <summary>
        /// Refreshes every cell in the grid from the controller state.
        /// </summary>
        void refreshGrid()
        {
            for (int row = 0; row < GridPanel::BOARD_SIZE; row++)
            {
                for (int col = 0; col < GridPanel::BOARD_SIZE; col++)
                {
                    gridPanel->setCellValue(
                        row,
                        col,
                        controller->getValue(row, col),
                        controller->isFixed(row, col)
                    );
                }
            }
        }

        /// <summary>
        /// Refreshes the puzzle label from the controller state.
        /// </summary>
        void refreshPuzzleLabel()
        {
            labelPuzzle->Text = "PUZZLE " + controller->getCurrentPuzzleNumber().ToString();
        }

        /// <summary>
        /// Refreshes the timer label from the controller state.
        /// </summary>
        void refreshTimerLabel()
        {
            labelTimer->Text = formatElapsedTime(controller->getElapsedSeconds());
        }

        /// <summary>
        /// Advances to the next puzzle if one is available.
        /// </summary>
        /// <returns>True if another puzzle was started; otherwise, false.</returns>
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