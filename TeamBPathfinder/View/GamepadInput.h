#pragma once

#pragma managed(push, off)
#include "../Controller/GameController.h"
#pragma managed(pop)

#include "GridPanel.h"

namespace TeamBPathfinder {

    using namespace System;

    public ref class GamepadInput {
    private:
        GridPanel^ gridPanel;
        Controller::GameController* controller;

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

    public:
        GamepadInput(GridPanel^ grid, Controller::GameController* ctrl)
        {
            gridPanel = grid;
            controller = ctrl;
        }

        void moveSelection(int rowDelta, int colDelta)
        {
            int newRow = gridPanel->getSelectedRow() + rowDelta;
            int newCol = gridPanel->getSelectedCol() + colDelta;

            if (newRow < 0 || newRow >= GridPanel::BOARD_SIZE) return;
            if (newCol < 0 || newCol >= GridPanel::BOARD_SIZE) return;

            // Notice we do NOT clear ActiveControl here anymore. 
            // The form handles its own ActiveControl clearing when necessary.
            gridPanel->selectCell(newRow, newCol);
            gridPanel->focusSelectedCell();
        }

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