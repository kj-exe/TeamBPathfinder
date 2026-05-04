#pragma once

#pragma managed(push, off)
#include "../Engine/GameEngine.h"
#include "../Model/UserSettings.h"
#pragma managed(pop)

namespace TeamBPathfinder {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;

    public delegate void CellInputHandler(int row, int col, String^ text);

    public ref class GridPanel : public Panel
    {
    public:
        static const int BOARD_SIZE = 8;
        static const int CELL_SIZE = 55;
        static const int TEXTBOX_HEIGHT = 26;

        static initonly Color LCD_LIGHTEST = Color::FromArgb(155, 188, 15);
        static initonly Color LCD_LIGHT = Color::FromArgb(139, 172, 15);
        static initonly Color LCD_DARK = Color::FromArgb(48, 98, 48);
        static initonly Color LCD_INK = Color::FromArgb(15, 56, 15);

        event CellInputHandler^ OnCellInput;

        GridPanel()
        {
            cellPanels = gcnew array<Panel^, 2>(BOARD_SIZE, BOARD_SIZE);
            cellTextBoxes = gcnew array<TextBox^, 2>(BOARD_SIZE, BOARD_SIZE);

            cellColor = LCD_LIGHTEST;
            fixedCellColor = LCD_DARK;
            numberColor = LCD_INK;
            selectedRow = 0;
            selectedCol = 0;

            this->Size = Drawing::Size(
                BOARD_SIZE * CELL_SIZE + 2,
                BOARD_SIZE * CELL_SIZE + 2
            );
            this->BorderStyle = System::Windows::Forms::BorderStyle::None;
            this->BackColor = LCD_LIGHTEST;
            this->DoubleBuffered = true;
            this->SetStyle(
                ControlStyles::OptimizedDoubleBuffer |
                ControlStyles::AllPaintingInWmPaint |
                ControlStyles::UserPaint |
                ControlStyles::ResizeRedraw,
                true);

            BuildCells();
        }

        void ApplySettings(UserSettings* settings)
        {
            cellColor = Color::FromArgb(settings->getCellColor());
            fixedCellColor = Color::FromArgb(settings->getFixedCellColor());
            numberColor = Color::FromArgb(settings->getNumberColor());

            for (int row = 0; row < BOARD_SIZE; row++)
                for (int col = 0; col < BOARD_SIZE; col++)
                {
                    Panel^ p = cellPanels[row, col];
                    TextBox^ tb = cellTextBoxes[row, col];
                    if (p == nullptr || tb == nullptr) continue;

                    bool isFixedCell = tb->ReadOnly;
                    Color tile = isFixedCell ? fixedCellColor : cellColor;
                    p->BackColor = tile;
                    tb->BackColor = tile;
                    tb->ForeColor = isFixedCell ? LCD_INK : numberColor;
                }
            this->Invalidate();
        }

        void UpdateCell(int row, int col, int value, bool isFixed)
        {
            Panel^ panel = cellPanels[row, col];
            TextBox^ textBox = cellTextBoxes[row, col];

            if (value > 0)
                textBox->Text = value.ToString();
            else
                textBox->Text = "";

            if (isFixed)
            {
                textBox->ReadOnly = true;
                panel->BackColor = fixedCellColor;
                textBox->BackColor = fixedCellColor;
                textBox->ForeColor = LCD_LIGHTEST;
                textBox->Font = (gcnew Drawing::Font(L"Consolas", 16, FontStyle::Bold));
            }
            else
            {
                textBox->ReadOnly = false;
                panel->BackColor = cellColor;
                textBox->BackColor = cellColor;
                textBox->ForeColor = numberColor;
                textBox->Font = (gcnew Drawing::Font(L"Consolas", 16, FontStyle::Bold));
            }
            panel->Invalidate();
        }

        void ClearCell(int row, int col)
        {
            cellTextBoxes[row, col]->Text = "";
            cellPanels[row, col]->BackColor = cellColor;
            cellTextBoxes[row, col]->BackColor = cellColor;
            cellPanels[row, col]->Invalidate();
        }

        void HighlightCellError(int row, int col)
        {
            Color err = LCD_DARK;
            cellPanels[row, col]->BackColor = err;
            cellTextBoxes[row, col]->BackColor = err;
            cellTextBoxes[row, col]->ForeColor = LCD_LIGHTEST;
            cellPanels[row, col]->Invalidate();
        }

        void ResetCellColor(int row, int col)
        {
            cellPanels[row, col]->BackColor = cellColor;
            cellTextBoxes[row, col]->BackColor = cellColor;
            cellTextBoxes[row, col]->ForeColor = numberColor;
            cellPanels[row, col]->Invalidate();
        }

        void SetSelectedCell(int row, int col)
        {
            if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
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

        int GetSelectedRow() { return selectedRow; }
        int GetSelectedCol() { return selectedCol; }

        void FocusSelectedCell()
        {
            if (selectedRow >= 0 && selectedRow < BOARD_SIZE && selectedCol >= 0 && selectedCol < BOARD_SIZE)
            {
                TextBox^ textBox = cellTextBoxes[selectedRow, selectedCol];
                if (!textBox->ReadOnly)
                    textBox->Focus();
            }
        }

    protected:
        virtual void OnPaint(PaintEventArgs^ e) override
        {
            Panel::OnPaint(e);

            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::None;

            int total = BOARD_SIZE * CELL_SIZE;

            Pen^ inkThick = gcnew Pen(LCD_INK, 2.0f);
            g->DrawRectangle(inkThick, 0, 0, total, total);

            Pen^ ink = gcnew Pen(LCD_INK, 1.0f);
            for (int i = 1; i < BOARD_SIZE; i++)
            {
                int p = i * CELL_SIZE;
                g->DrawLine(ink, p, 1, p, total - 1);
                g->DrawLine(ink, 1, p, total - 1, p);
            }

            delete ink;
            delete inkThick;
        }

    private:
        array<Panel^, 2>^ cellPanels;
        array<TextBox^, 2>^ cellTextBoxes;
        Color cellColor;
        Color fixedCellColor;
        Color numberColor;
        int selectedRow;
        int selectedCol;

        void BuildCells()
        {
            for (int row = 0; row < BOARD_SIZE; row++)
                for (int col = 0; col < BOARD_SIZE; col++)
                    AddCell(row, col);
        }

        void AddCell(int row, int col)
        {
            Panel^ panel = gcnew Panel();
            panel->Size = Drawing::Size(CELL_SIZE - 1, CELL_SIZE - 1);
            panel->Location = Point(col * CELL_SIZE + 1, row * CELL_SIZE + 1);
            panel->BorderStyle = System::Windows::Forms::BorderStyle::None;
            panel->BackColor = cellColor;
            panel->Tag = gcnew Point(row, col);
            panel->Click += gcnew EventHandler(this, &GridPanel::Panel_Click);
            panel->Paint += gcnew PaintEventHandler(this, &GridPanel::CellPanel_Paint);

            TextBox^ textBox = gcnew TextBox();
            textBox->Size = Drawing::Size(CELL_SIZE - 8, TEXTBOX_HEIGHT);
            textBox->Location = Point(3, (CELL_SIZE - 1 - TEXTBOX_HEIGHT) / 2);
            textBox->TextAlign = HorizontalAlignment::Center;
            textBox->MaxLength = 2;
            textBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
            textBox->Font = (gcnew Drawing::Font(L"Consolas", 16, FontStyle::Bold));
            textBox->BackColor = cellColor;
            textBox->ForeColor = numberColor;
            textBox->TabStop = false;
            textBox->Tag = gcnew Point(row, col);
            textBox->Leave += gcnew EventHandler(this, &GridPanel::Cell_Leave);

            panel->Controls->Add(textBox);

            cellPanels[row, col] = panel;
            cellTextBoxes[row, col] = textBox;
            this->Controls->Add(panel);
        }

        void Panel_Click(Object^ sender, EventArgs^ e)
        {
            Panel^ panel = safe_cast<Panel^>(sender);
            Point^ position = safe_cast<Point^>(panel->Tag);
            SetSelectedCell(position->X, position->Y);
            TextBox^ textBox = cellTextBoxes[position->X, position->Y];
            if (!textBox->ReadOnly)
                textBox->Focus();
        }

        void CellPanel_Paint(Object^ sender, PaintEventArgs^ e)
        {
            Panel^ panel = safe_cast<Panel^>(sender);
            Point^ position = safe_cast<Point^>(panel->Tag);

            if (position->X == selectedRow && position->Y == selectedCol)
            {
                Pen^ selPen = gcnew Pen(LCD_INK, 3.0f);

                e->Graphics->DrawRectangle(selPen, 1, 1, panel->Width - 3, panel->Height - 3);

                delete selPen;
            }
        }

        void Cell_Leave(Object^ sender, EventArgs^ e)
        {
            TextBox^ cell = safe_cast<TextBox^>(sender);
            if (cell->ReadOnly)
                return;

            Point^ position = safe_cast<Point^>(cell->Tag);
            OnCellInput(position->X, position->Y, cell->Text->Trim());
        }
    };
}