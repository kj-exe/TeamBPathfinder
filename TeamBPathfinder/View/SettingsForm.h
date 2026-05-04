#pragma once

#pragma managed(push, off)
#include "../Model/UserSettings.h"
#pragma managed(pop)

namespace TeamBPathfinder
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class SettingsForm : public Form
	{
	public:
		SettingsForm(UserSettings* settings)
		{
			this->settings = settings;
			BuildUI();
		}

	private:
		UserSettings* settings;

		Panel^ cellColorPreview;
		Panel^ numberColorPreview;
		Panel^ fixedColorPreview;

		void BuildUI()
		{
			this->Text = "Settings";
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->StartPosition = FormStartPosition::CenterParent;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->ClientSize = Drawing::Size(360, 240);

			Drawing::Font^ labelFont = gcnew Drawing::Font(L"Segoe UI", 10);

			Label^ cellLabel = gcnew Label();
			cellLabel->Text = L"Cell Color:";
			cellLabel->Font = labelFont;
			cellLabel->Location = Point(20, 20);
			cellLabel->Size = Drawing::Size(120, 24);
			this->Controls->Add(cellLabel);

			cellColorPreview = gcnew Panel();
			cellColorPreview->Location = Point(150, 18);
			cellColorPreview->Size = Drawing::Size(60, 28);
			cellColorPreview->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			cellColorPreview->BackColor = Color::FromArgb(settings->getCellColor());
			this->Controls->Add(cellColorPreview);

			Button^ cellButton = gcnew Button();
			cellButton->Text = L"Choose...";
			cellButton->Location = Point(220, 18);
			cellButton->Size = Drawing::Size(120, 28);
			cellButton->Click += gcnew EventHandler(this, &SettingsForm::OnPickCellColor);
			this->Controls->Add(cellButton);

			Label^ numberLabel = gcnew Label();
			numberLabel->Text = L"Number Color:";
			numberLabel->Font = labelFont;
			numberLabel->Location = Point(20, 60);
			numberLabel->Size = Drawing::Size(120, 24);
			this->Controls->Add(numberLabel);

			numberColorPreview = gcnew Panel();
			numberColorPreview->Location = Point(150, 58);
			numberColorPreview->Size = Drawing::Size(60, 28);
			numberColorPreview->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			numberColorPreview->BackColor = Color::FromArgb(settings->getNumberColor());
			this->Controls->Add(numberColorPreview);

			Button^ numberButton = gcnew Button();
			numberButton->Text = L"Choose...";
			numberButton->Location = Point(220, 58);
			numberButton->Size = Drawing::Size(120, 28);
			numberButton->Click += gcnew EventHandler(this, &SettingsForm::OnPickNumberColor);
			this->Controls->Add(numberButton);

			Label^ fixedLabel = gcnew Label();
			fixedLabel->Text = L"Fixed Cell Color:";
			fixedLabel->Font = labelFont;
			fixedLabel->Location = Point(20, 100);
			fixedLabel->Size = Drawing::Size(120, 24);
			this->Controls->Add(fixedLabel);

			fixedColorPreview = gcnew Panel();
			fixedColorPreview->Location = Point(150, 98);
			fixedColorPreview->Size = Drawing::Size(60, 28);
			fixedColorPreview->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			fixedColorPreview->BackColor = Color::FromArgb(settings->getFixedCellColor());
			this->Controls->Add(fixedColorPreview);

			Button^ fixedButton = gcnew Button();
			fixedButton->Text = L"Choose...";
			fixedButton->Location = Point(220, 98);
			fixedButton->Size = Drawing::Size(120, 28);
			fixedButton->Click += gcnew EventHandler(this, &SettingsForm::OnPickFixedColor);
			this->Controls->Add(fixedButton);

			Button^ resetButton = gcnew Button();
			resetButton->Text = L"Reset to Defaults";
			resetButton->Location = Point(20, 160);
			resetButton->Size = Drawing::Size(160, 36);
			resetButton->Click += gcnew EventHandler(this, &SettingsForm::OnResetClicked);
			this->Controls->Add(resetButton);

			Button^ closeButton = gcnew Button();
			closeButton->Text = L"Close";
			closeButton->Location = Point(220, 160);
			closeButton->Size = Drawing::Size(120, 36);
			closeButton->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->Controls->Add(closeButton);
			this->AcceptButton = closeButton;
		}

		void OnPickCellColor(Object^ sender, EventArgs^ e)
		{
			ColorDialog^ dialog = gcnew ColorDialog();
			dialog->Color = Color::FromArgb(settings->getCellColor());
			if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				settings->setCellColor(dialog->Color.ToArgb());
				cellColorPreview->BackColor = dialog->Color;
			}
		}

		void OnPickNumberColor(Object^ sender, EventArgs^ e)
		{
			ColorDialog^ dialog = gcnew ColorDialog();
			dialog->Color = Color::FromArgb(settings->getNumberColor());
			if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				settings->setNumberColor(dialog->Color.ToArgb());
				numberColorPreview->BackColor = dialog->Color;
			}
		}

		void OnPickFixedColor(Object^ sender, EventArgs^ e)
		{
			ColorDialog^ dialog = gcnew ColorDialog();
			dialog->Color = Color::FromArgb(settings->getFixedCellColor());
			if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				settings->setFixedCellColor(dialog->Color.ToArgb());
				fixedColorPreview->BackColor = dialog->Color;
			}
		}

		void OnResetClicked(Object^ sender, EventArgs^ e)
		{
			settings->resetToDefaults();
			cellColorPreview->BackColor = Color::FromArgb(settings->getCellColor());
			numberColorPreview->BackColor = Color::FromArgb(settings->getNumberColor());
			fixedColorPreview->BackColor = Color::FromArgb(settings->getFixedCellColor());
		}
	};
}