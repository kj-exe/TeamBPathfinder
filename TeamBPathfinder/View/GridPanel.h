#pragma once

#pragma managed(push, off)
#include "../Engine/GameEngine.h"
#pragma managed(pop)

namespace TeamBPathfinder {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;
    using namespace Model;

    /// <summary>
    /// Handles cell input submitted from the grid.
    /// </summary>
    /// <param name="row">The row index of the edited cell.</param>
    /// <param name="col">The column index of the edited cell.</param>
    /// <param name="text">The text entered in the cell.</param>
    public delegate void CellInputHandler(int row, int col, String^ text);

    /// <summary>
    /// Stores the colors used to style the puzzle grid.
    /// </summary>
    public ref class GridTheme
    {
    public:
        Color CellColor;
        Color FixedCellColor;
        Color NumberColor;
        Color ErrorColor;
        Color FixedTextColor;
        Color ErrorTextColor;
        Color BorderColor;
        Color SelectedBorderColor;
    };

    /// <summary>
    /// Displays the puzzle grid, tracks selected cells, applies cell styles, and reports cell input.
    /// </summary>
    public ref class GridPanel : public Panel
    {
    public:
        static const int BOARD_SIZE = 8;
        static const int CELL_SIZE = 55;
        static const int TEXTBOX_HEIGHT = 26;
        static const int GRID_BORDER_OFFSET = 1;
        static const int TEXTBOX_HORIZONTAL_PADDING = 8;
        static const int TEXTBOX_LEFT_OFFSET = 3;
        static const int OUTER_BORDER_WIDTH = 2;
        static const int SELECTED_BORDER_WIDTH = 3;

        static initonly Color LCD_LIGHTEST = Color::FromArgb(155, 188, 15);
        static initonly Color LCD_LIGHT = Color::FromArgb(139, 172, 15);
        static initonly Color LCD_DARK = Color::FromArgb(48, 98, 48);
        static initonly Color LCD_INK = Color::FromArgb(15, 56, 15);

        event CellInputHandler^ CellInputSubmitted;

        /// <summary>
        /// Initializes a new grid panel with default styling and empty cells.
        /// </summary>
        GridPanel()
        {
            cellPanels = gcnew array<Panel^, 2>(BOARD_SIZE, BOARD_SIZE);
            cellTextBoxes = gcnew array<TextBox^, 2>(BOARD_SIZE, BOARD_SIZE);
            fixedCells = gcnew array<bool, 2>(BOARD_SIZE, BOARD_SIZE);
            cellFont = gcnew Drawing::Font(L"Consolas", 16, FontStyle::Bold);

            isProgrammaticUpdate = false;

            theme = gcnew GridTheme();
            theme->CellColor = LCD_LIGHTEST;
            theme->FixedCellColor = LCD_DARK;
            theme->NumberColor = LCD_INK;
            theme->ErrorColor = LCD_DARK;
            theme->FixedTextColor = LCD_LIGHTEST;
            theme->ErrorTextColor = LCD_LIGHTEST;
            theme->BorderColor = LCD_INK;
            theme->SelectedBorderColor = LCD_INK;

            selectedRow = 0;
            selectedCol = 0;

            this->Size = Drawing::Size(
                BOARD_SIZE * CELL_SIZE + OUTER_BORDER_WIDTH,
                BOARD_SIZE * CELL_SIZE + OUTER_BORDER_WIDTH
            );

            this->BorderStyle = System::Windows::Forms::BorderStyle::None;
            this->BackColor = theme->CellColor;
            this->DoubleBuffered = true;

            this->SetStyle(
                ControlStyles::OptimizedDoubleBuffer |
                ControlStyles::AllPaintingInWmPaint |
                ControlStyles::UserPaint |
                ControlStyles::ResizeRedraw,
                true);

            BuildCells();
        }

        /// <summary>
        /// Applies a visual theme to the grid.
        /// </summary>
        /// <param name="newTheme">The theme to apply.</param>
        void applyTheme(GridTheme^ newTheme)
        {
            if (newTheme == nullptr)
                return;

            theme = newTheme;
            this->BackColor = theme->CellColor;

            for (int row = 0; row < BOARD_SIZE; row++)
            {
                for (int col = 0; col < BOARD_SIZE; col++)
                {
                    if (fixedCells[row, col])
                        applyFixedCellStyle(row, col);
                    else
                        applyEditableCellStyle(row, col);
                }
            }

            this->Invalidate();
        }

        /// <summary>
        /// Sets the displayed value and fixed state of a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        /// <param name="value">The value to display, or 0 to clear the cell.</param>
        /// <param name="isFixed">Whether the cell should be read-only.</param>
        void setCellValue(int row, int col, int value, bool isFixed)
        {
            TextBox^ textBox = getCellTextBox(row, col);
            if (textBox == nullptr)
                return;

            isProgrammaticUpdate = true;

            fixedCells[row, col] = isFixed;
            textBox->ReadOnly = isFixed;
            textBox->Font = cellFont;

            if (value > 0)
                textBox->Text = value.ToString();
            else
                textBox->Text = "";

            isProgrammaticUpdate = false;

            if (isFixed)
                applyFixedCellStyle(row, col);
            else
                applyEditableCellStyle(row, col);
        }

        /// <summary>
        /// Clears the displayed value from a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void clearCellValue(int row, int col)
        {
            TextBox^ textBox = getCellTextBox(row, col);
            if (textBox == nullptr)
                return;

            isProgrammaticUpdate = true;
            textBox->Text = "";
            isProgrammaticUpdate = false;

            fixedCells[row, col] = false;
            textBox->ReadOnly = false;

            applyEditableCellStyle(row, col);
        }

        /// <summary>
        /// Applies the error style to a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void showCellError(int row, int col)
        {
            applyErrorCellStyle(row, col);
        }

        /// <summary>
        /// Restores the normal style for a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void resetCellStyle(int row, int col)
        {
            if (!isValidCellPosition(row, col))
                return;

            if (fixedCells[row, col])
                applyFixedCellStyle(row, col);
            else
                applyEditableCellStyle(row, col);
        }

        /// <summary>
        /// Selects the cell at the specified row and column.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void selectCell(int row, int col)
        {
            if (!isValidCellPosition(row, col))
                return;

            int oldRow = selectedRow;
            int oldCol = selectedCol;

            selectedRow = row;
            selectedCol = col;

            if (cellPanels[oldRow, oldCol] != nullptr)
                cellPanels[oldRow, oldCol]->Invalidate();

            if (cellPanels[selectedRow, selectedCol] != nullptr)
                cellPanels[selectedRow, selectedCol]->Invalidate();
        }

        /// <summary>
        /// Gets the row index of the selected cell.
        /// </summary>
        /// <returns>The selected row index.</returns>
        int getSelectedRow()
        {
            return selectedRow;
        }

        /// <summary>
        /// Gets the column index of the selected cell.
        /// </summary>
        /// <returns>The selected column index.</returns>
        int getSelectedCol()
        {
            return selectedCol;
        }

        /// <summary>
        /// Focuses the selected cell if it is editable.
        /// </summary>
        void focusSelectedCell()
        {
            TextBox^ textBox = getCellTextBox(selectedRow, selectedCol);
            if (textBox == nullptr)
                return;

            if (!textBox->ReadOnly)
                textBox->Focus();
        }

    protected:
        /// <summary>
        /// Paints the grid border and grid lines.
        /// </summary>
        /// <param name="e">The paint event arguments.</param>
        virtual void OnPaint(PaintEventArgs^ e) override
        {
            Panel::OnPaint(e);

            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::None;

            int total = BOARD_SIZE * CELL_SIZE;

            Pen^ inkThick = gcnew Pen(theme->BorderColor, OUTER_BORDER_WIDTH);
            g->DrawRectangle(inkThick, 0, 0, total, total);

            Pen^ ink = gcnew Pen(theme->BorderColor, 1.0f);

            for (int i = 1; i < BOARD_SIZE; i++)
            {
                int position = i * CELL_SIZE;
                g->DrawLine(ink, position, 1, position, total - 1);
                g->DrawLine(ink, 1, position, total - 1, position);
            }

            delete ink;
            delete inkThick;
        }

    private:
        array<Panel^, 2>^ cellPanels;
        array<TextBox^, 2>^ cellTextBoxes;
        array<bool, 2>^ fixedCells;

        Drawing::Font^ cellFont;
        GridTheme^ theme;

        int selectedRow;
        int selectedCol;

        bool isProgrammaticUpdate;

        /// <summary>
        /// Determines whether a row and column are valid grid positions.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        /// <returns>True if the position is valid; otherwise, false.</returns>
        bool isValidCellPosition(int row, int col)
        {
            return row >= 0 && row < BOARD_SIZE &&
                col >= 0 && col < BOARD_SIZE;
        }

        /// <summary>
        /// Gets the panel for the specified cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        /// <returns>The cell panel, or nullptr if the position is invalid.</returns>
        Panel^ getCellPanel(int row, int col)
        {
            if (!isValidCellPosition(row, col))
                return nullptr;

            return cellPanels[row, col];
        }

        /// <summary>
        /// Gets the text box for the specified cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        /// <returns>The cell text box, or nullptr if the position is invalid.</returns>
        TextBox^ getCellTextBox(int row, int col)
        {
            if (!isValidCellPosition(row, col))
                return nullptr;

            return cellTextBoxes[row, col];
        }

        /// <summary>
        /// Applies background and text colors to a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        /// <param name="backColor">The cell background color.</param>
        /// <param name="foreColor">The cell text color.</param>
        void applyCellStyle(int row, int col, Color backColor, Color foreColor)
        {
            Panel^ panel = getCellPanel(row, col);
            TextBox^ textBox = getCellTextBox(row, col);

            if (panel == nullptr || textBox == nullptr)
                return;

            panel->BackColor = backColor;
            textBox->BackColor = backColor;
            textBox->ForeColor = foreColor;

            panel->Invalidate();
        }

        /// <summary>
        /// Applies the editable style to a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void applyEditableCellStyle(int row, int col)
        {
            applyCellStyle(row, col, theme->CellColor, theme->NumberColor);
        }

        /// <summary>
        /// Applies the fixed-cell style to a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void applyFixedCellStyle(int row, int col)
        {
            applyCellStyle(row, col, theme->FixedCellColor, theme->FixedTextColor);
        }

        /// <summary>
        /// Applies the error style to a cell.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void applyErrorCellStyle(int row, int col)
        {
            applyCellStyle(row, col, theme->ErrorColor, theme->ErrorTextColor);
        }

        /// <summary>
        /// Builds all grid cells.
        /// </summary>
        void BuildCells()
        {
            for (int row = 0; row < BOARD_SIZE; row++)
            {
                for (int col = 0; col < BOARD_SIZE; col++)
                {
                    AddCell(row, col);
                }
            }
        }

        /// <summary>
        /// Adds one cell to the grid.
        /// </summary>
        /// <param name="row">The row index.</param>
        /// <param name="col">The column index.</param>
        void AddCell(int row, int col)
        {
            Panel^ panel = gcnew Panel();

            panel->Size = Drawing::Size(
                CELL_SIZE - GRID_BORDER_OFFSET,
                CELL_SIZE - GRID_BORDER_OFFSET
            );

            panel->Location = Point(
                col * CELL_SIZE + GRID_BORDER_OFFSET,
                row * CELL_SIZE + GRID_BORDER_OFFSET
            );

            panel->BorderStyle = System::Windows::Forms::BorderStyle::None;
            panel->BackColor = theme->CellColor;
            panel->Tag = gcnew Point(row, col);

            panel->Click += gcnew EventHandler(this, &GridPanel::Panel_Click);
            panel->Paint += gcnew PaintEventHandler(this, &GridPanel::CellPanel_Paint);

            TextBox^ textBox = gcnew TextBox();

            textBox->Size = Drawing::Size(
                CELL_SIZE - TEXTBOX_HORIZONTAL_PADDING,
                TEXTBOX_HEIGHT
            );

            textBox->Location = Point(
                TEXTBOX_LEFT_OFFSET,
                (CELL_SIZE - GRID_BORDER_OFFSET - TEXTBOX_HEIGHT) / 2
            );

            textBox->TextAlign = HorizontalAlignment::Center;
            textBox->MaxLength = 2;
            textBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
            textBox->Font = cellFont;
            textBox->BackColor = theme->CellColor;
            textBox->ForeColor = theme->NumberColor;
            textBox->TabStop = false;
            textBox->Tag = gcnew Point(row, col);

            textBox->Leave += gcnew EventHandler(this, &GridPanel::Cell_Leave);

            panel->Controls->Add(textBox);

            cellPanels[row, col] = panel;
            cellTextBoxes[row, col] = textBox;
            fixedCells[row, col] = false;

            this->Controls->Add(panel);
        }

        /// <summary>
        /// Handles clicks on a cell panel.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        void Panel_Click(Object^ sender, EventArgs^ e)
        {
            Panel^ panel = safe_cast<Panel^>(sender);
            Point^ position = safe_cast<Point^>(panel->Tag);

            selectCell(position->X, position->Y);
            focusSelectedCell();
        }

        /// <summary>
        /// Paints the selected-cell border on a cell panel.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The paint event arguments.</param>
        void CellPanel_Paint(Object^ sender, PaintEventArgs^ e)
        {
            Panel^ panel = safe_cast<Panel^>(sender);
            Point^ position = safe_cast<Point^>(panel->Tag);

            if (position->X != selectedRow || position->Y != selectedCol)
                return;

            Pen^ selectedPen = gcnew Pen(
                theme->SelectedBorderColor,
                SELECTED_BORDER_WIDTH
            );

            e->Graphics->DrawRectangle(
                selectedPen,
                1,
                1,
                panel->Width - 3,
                panel->Height - 3
            );

            delete selectedPen;
        }

        /// <summary>
        /// Handles text changes from an editable grid cell.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">The event arguments.</param>
        void Cell_Leave(Object^ sender, EventArgs^ e)
        {
            TextBox^ cell = safe_cast<TextBox^>(sender);

            if (cell->ReadOnly)
                return;

            Point^ position = safe_cast<Point^>(cell->Tag);

            CellInputSubmitted(
                position->X,
                position->Y,
                cell->Text->Trim()
            );
        }
    };
}