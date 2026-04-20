#pragma once

#pragma managed(push, off)
#include "GameEngine.h"
#pragma managed(pop)

namespace TeamBPathfinder {

	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public delegate void CellInputHandler(int row, int col, String^ text);

	public ref class GridPanel : public Panel
	{
	public:
		static const int BOARD_SIZE = 8;
		static const int CELL_SIZE = 55;

		event CellInputHandler^ OnCellInput;

		GridPanel()
		{
			cells = gcnew array<TextBox^, 2>(BOARD_SIZE, BOARD_SIZE);
			this->Size = Drawing::Size(
				BOARD_SIZE * CELL_SIZE + 2,
				BOARD_SIZE * CELL_SIZE + 2
			);
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			BuildCells();
		}

		void UpdateCell(int row, int col, int value, bool isFixed)
		{
			TextBox^ cell = cells[row, col];

			if (value > 0)
				cell->Text = value.ToString();
			else
				cell->Text = "";

			if (isFixed)
			{
				cell->ReadOnly = true;
				cell->BackColor = Color::FromArgb(220, 220, 220);
				cell->ForeColor = Color::FromArgb(40, 40, 40);
				cell->Font = (gcnew Drawing::Font(L"Segoe UI", 13, FontStyle::Bold));
			}
			else
			{
				cell->ReadOnly = false;
				cell->BackColor = Color::White;
				cell->ForeColor = Color::FromArgb(60, 60, 60);
				cell->Font = (gcnew Drawing::Font(L"Segoe UI", 13));
			}
		}

		void ClearCell(int row, int col)
		{
			cells[row, col]->Text = "";
			cells[row, col]->BackColor = Color::White;
		}

		void HighlightCellError(int row, int col)
		{
			cells[row, col]->BackColor = Color::FromArgb(255, 200, 200);
		}

		void ResetCellColor(int row, int col)
		{
			cells[row, col]->BackColor = Color::White;
		}

	private:
		array<TextBox^, 2>^ cells;

		void BuildCells()
		{
			for (int row = 0; row < BOARD_SIZE; row++)
				for (int col = 0; col < BOARD_SIZE; col++)
					AddCell(row, col);
		}

		void AddCell(int row, int col)
		{
			TextBox^ cell = gcnew TextBox();
			cell->Size = Drawing::Size(CELL_SIZE, CELL_SIZE);
			cell->Location = Point(col * CELL_SIZE, row * CELL_SIZE);
			cell->Multiline = true;
			cell->TextAlign = HorizontalAlignment::Center;
			cell->MaxLength = 2;
			cell->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			cell->Font = (gcnew Drawing::Font(L"Segoe UI", 13));
			cell->Tag = gcnew Point(row, col);
			cell->Leave += gcnew EventHandler(this, &GridPanel::Cell_Leave);

			cells[row, col] = cell;
			this->Controls->Add(cell);
		}

		void Cell_Leave(Object^ sender, EventArgs^ e)
		{
			TextBox^ cell = safe_cast<TextBox^>(sender);
			Point^ position = safe_cast<Point^>(cell->Tag);
			OnCellInput(position->X, position->Y, cell->Text->Trim());
		}
	};
}
