#pragma once

#include "../Model/Scoreboard.h"
#include "../Model/ScoreEntry.h"

namespace TeamBPathfinder
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class ScoreboardForm : public Form
	{
	public:
		ScoreboardForm(const Scoreboard& scoreboard)
		{
			this->Text = "Scoreboard";
			this->Width = 500;
			this->Height = 350;
			this->StartPosition = FormStartPosition::CenterParent;

			DataGridView^ grid = gcnew DataGridView();
			grid->Dock = DockStyle::Fill;
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

			const std::vector<ScoreEntry>& scores = scoreboard.getScores();

			for (size_t i = 0; i < scores.size(); i++)
			{
				const ScoreEntry& entry = scores[i];

				int seconds = entry.getSeconds();
				int minutes = seconds / 60;
				int remaining = seconds % 60;

				String^ time =
					minutes.ToString("00") + ":" + remaining.ToString("00");

				grid->Rows->Add(
					(i + 1).ToString(),
					gcnew String(entry.getPlayerName().c_str()),
					entry.getPuzzleNumber().ToString(),
					time
				);
			}

			this->Controls->Add(grid);
		}
	};
}