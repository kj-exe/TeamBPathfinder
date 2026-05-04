#pragma once

#pragma managed(push, off)
#include "../Engine/GameEngine.h"
#include "../Model/UserSettings.h"
#pragma managed(pop)

namespace TeamBPathfinder {

	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace Model;

	public delegate void CellInputHandler(int row, int col, String^ text);

	public ref class GridPanel : public Panel
	{
	public:
		static const int BOARD_SIZE = 8;
		static const int CELL_SIZE = 55;
		static const int TEXTBOX_HEIGHT = 26;

		event CellInputHandler^ OnCellInput;

		GridPanel()
		{
			cellPanels = gcnew array<Panel^, 2>(BOARD_SIZE, BOARD_SIZE);
			cellTextBoxes = gcnew array<TextBox^, 2>(BOARD_SIZE, BOARD_SIZE);

			cellColor = Color::White;
			fixedCellColor = Color::FromArgb(220, 220, 220);
			numberColor = Color::FromArgb(60, 60, 60);

			this->Size = Drawing::Size(
				BOARD_SIZE * CELL_SIZE + 2,
				BOARD_SIZE * CELL_SIZE + 2
			);
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			BuildCells();
		}

		void ApplySettings(UserSettings* settings)
		{
			cellColor = Color::FromArgb(settings->getCellColor());
			fixedCellColor = Color::FromArgb(settings->getFixedCellColor());
			numberColor = Color::FromArgb(settings->getNumberColor());
		}

		void UpdateCell(int row, int col, int value, bool isFixed)
		{
			Panel^ panel = cellPanels[row, col];
			TextBox^ textBox = cellTextBoxes[row, col];

			if (value > 0)
				textBox->Text = value.ToString();
			else
				textBox->Text = "";

			textBox->ForeColor = numberColor;

			if (isFixed)
			{
				textBox->ReadOnly = true;
				panel->BackColor = fixedCellColor;
				textBox->BackColor = fixedCellColor;
				textBox->Font = (gcnew Drawing::Font(L"Segoe UI", 13, FontStyle::Bold));
			}
			else
			{
				textBox->ReadOnly = false;
				panel->BackColor = cellColor;
				textBox->BackColor = cellColor;
				textBox->Font = (gcnew Drawing::Font(L"Segoe UI", 13));
			}
		}

		void ClearCell(int row, int col)
		{
			cellTextBoxes[row, col]->Text = "";
			cellPanels[row, col]->BackColor = cellColor;
			cellTextBoxes[row, col]->BackColor = cellColor;
		}

		void HighlightCellError(int row, int col)
		{
			cellPanels[row, col]->BackColor = Color::FromArgb(255, 200, 200);
			cellTextBoxes[row, col]->BackColor = Color::FromArgb(255, 200, 200);
		}

		void ResetCellColor(int row, int col)
		{
			cellPanels[row, col]->BackColor = cellColor;
			cellTextBoxes[row, col]->BackColor = cellColor;
		}

	private:
		array<Panel^, 2>^ cellPanels;
		array<TextBox^, 2>^ cellTextBoxes;
		Color cellColor;
		Color fixedCellColor;
		Color numberColor;

		void BuildCells()
		{
			for (int row = 0; row < BOARD_SIZE; row++)
				for (int col = 0; col < BOARD_SIZE; col++)
					AddCell(row, col);
		}

		void AddCell(int row, int col)
		{
			Panel^ panel = gcnew Panel();
			panel->Size = Drawing::Size(CELL_SIZE, CELL_SIZE);
			panel->Location = Point(col * CELL_SIZE, row * CELL_SIZE);
			panel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			panel->BackColor = cellColor;
			panel->Tag = gcnew Point(row, col);
			panel->Click += gcnew EventHandler(this, &GridPanel::Panel_Click);

			TextBox^ textBox = gcnew TextBox();
			textBox->Size = Drawing::Size(CELL_SIZE - 6, TEXTBOX_HEIGHT);
			textBox->Location = Point(2, (CELL_SIZE - TEXTBOX_HEIGHT) / 2);
			textBox->TextAlign = HorizontalAlignment::Center;
			textBox->MaxLength = 2;
			textBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			textBox->Font = (gcnew Drawing::Font(L"Segoe UI", 13));
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
			TextBox^ textBox = cellTextBoxes[position->X, position->Y];
			if (!textBox->ReadOnly)
				textBox->Focus();
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