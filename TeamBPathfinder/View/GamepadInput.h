#pragma once

#pragma managed(push, off)
#include "../Controller/GameController.h"
#pragma managed(pop)

#include "GridPanel.h"

namespace TeamBPathfinder {

    using namespace System;

    /// <summary>
    /// Handles Game Boy-style directional and button input for the puzzle grid.
    /// </summary>
    public ref class GamepadInput {
    private:
        GridPanel^ gridPanel;
        Controller::GameController* controller;

        /// <summary>
        /// Finds the next editable empty cell starting from the given position.
        /// </summary>
        /// <param name="row">The starting row index, updated to the found row.</param>
        /// <param name="col">The starting column index, updated to the found column.</param>
        /// <returns>True if an editable empty cell is found; otherwise, false.</returns>
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

        /// <summary>
        /// Computes the smallest available number not currently used on the board.
        /// </summary>
        /// <returns>The next available number, or -1 if no number is available.</returns>
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

    public:
        /// <summary>
        /// Initializes a new gamepad input handler.
        /// </summary>
        /// <param name="grid">The grid panel controlled by gamepad input.</param>
        /// <param name="ctrl">The controller that provides and updates game state.</param>
        GamepadInput(GridPanel^ grid, Controller::GameController* ctrl)
        {
            gridPanel = grid;
            controller = ctrl;
        }

        /// <summary>
        /// Moves the selected grid cell by the given row and column offsets.
        /// </summary>
        /// <param name="rowDelta">The row movement amount.</param>
        /// <param name="colDelta">The column movement amount.</param>
        void moveSelection(int rowDelta, int colDelta)
        {
            int newRow = gridPanel->getSelectedRow() + rowDelta;
            int newCol = gridPanel->getSelectedCol() + colDelta;

            if (newRow < 0 || newRow >= GridPanel::BOARD_SIZE) return;
            if (newCol < 0 || newCol >= GridPanel::BOARD_SIZE) return;

            gridPanel->selectCell(newRow, newCol);
            gridPanel->focusSelectedCell();
        }

        /// <summary>
        /// Handles the A button by placing the next available number in the selected cell.
        /// </summary>
        void pressA()
        {
            if (controller->isCurrentPuzzleSolved()) return;

            int row = gridPanel->getSelectedRow();
            int col = gridPanel->getSelectedCol();

            if (controller->isFixed(row, col) || controller->getValue(row, col) > 0)
            {
                if (!findNextEmptyCell(row, col))
                    return;
                gridPanel->selectCell(row, col);
            }

            int nextNumber = computeNextAvailableNumber();
            if (nextNumber < 1) return;

            Controller::MoveResult result = controller->attemptMove(row, col, nextNumber);
            if (result == Controller::MoveResult::Accepted)
            {
                gridPanel->setCellValue(row, col, nextNumber, false);
                gridPanel->resetCellStyle(row, col);
            }
        }

        /// <summary>
        /// Handles the B button by clearing the selected cell if it is editable.
        /// </summary>
        void pressB()
        {
            if (controller->isCurrentPuzzleSolved()) return;

            int row = gridPanel->getSelectedRow();
            int col = gridPanel->getSelectedCol();

            if (controller->isFixed(row, col)) return;

            controller->clearCell(row, col);
            gridPanel->setCellValue(row, col, 0, false);
            gridPanel->resetCellStyle(row, col);
        }
    };
}