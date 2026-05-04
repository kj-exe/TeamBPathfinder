#pragma once

#include "../Model/Scoreboard.h"
#include "../Model/ScoreEntry.h"

namespace TeamBPathfinder
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace Model;

	public ref class ScoreboardForm : public Form
	{
	private:
		const Scoreboard* scoreboard;
		DataGridView^ grid;
		ComboBox^ sortComboBox;

	public:
		ScoreboardForm(const Scoreboard& scoreboard)
		{
			this->scoreboard = &scoreboard;

			this->Text = "Scoreboard";
			this->Width = 520;
			this->Height = 390;
			this->StartPosition = FormStartPosition::CenterParent;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;

			sortComboBox = gcnew ComboBox();
			sortComboBox->DropDownStyle = ComboBoxStyle::DropDownList;
			sortComboBox->Location = Point(16, 16);
			sortComboBox->Width = 220;
			sortComboBox->Items->Add("Sort by Time");
			sortComboBox->Items->Add("Sort by Puzzle Number");
			sortComboBox->SelectedIndex = 0;
			sortComboBox->SelectedIndexChanged += gcnew EventHandler(this, &ScoreboardForm::OnSortChanged);

			grid = gcnew DataGridView();
			grid->Location = Point(16, 52);
			grid->Size = Drawing::Size(470, 280);
			grid->ReadOnly = true;
			grid->AllowUserToAddRows = false;
			grid->RowHeadersVisible = false;
			grid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;

			grid->Font = gcnew Drawing::Font("Segoe UI", 10);
			grid->ColumnHeadersDefaultCellStyle->Font =
				gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);

			grid->Columns->Add("Rank", "Rank");
			grid->Columns->Add("Name", "Name");
			grid->Columns->Add("Puzzle", "Puzzle");
			grid->Columns->Add("Time", "Time");

			this->Controls->Add(sortComboBox);
			this->Controls->Add(grid);

			LoadScores();
		}

	private:
		void OnSortChanged(Object^ sender, EventArgs^ e)
		{
			LoadScores();
		}

		void LoadScores()
		{
			grid->Rows->Clear();

			ScoreSortMode sortMode = ScoreSortMode::Time;

			if (sortComboBox->SelectedIndex == 1)
			{
				sortMode = ScoreSortMode::PuzzleThenTime;
			}

			std::vector<ScoreEntry> scores = scoreboard->getSortedScores(sortMode);

			for (size_t i = 0; i < scores.size(); i++)
			{
				const ScoreEntry& entry = scores[i];

				int seconds = entry.getSeconds();
				int minutes = seconds / 60;
				int remaining = seconds % 60;

				String^ time = minutes.ToString("00") + ":" + remaining.ToString("00");

				grid->Rows->Add(
					(i + 1).ToString(),
					gcnew String(entry.getPlayerName().c_str()),
					entry.getPuzzleNumber().ToString(),
					time
				);
			}
		}
	};
}
