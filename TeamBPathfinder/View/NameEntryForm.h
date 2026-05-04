#pragma once

namespace TeamBPathfinder
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class NameEntryForm : public Form
	{
	private:
		TextBox^ nameTextBox;
		Button^ saveButton;
		Button^ cancelButton;

	public:
		NameEntryForm()
		{
			this->Text = "New High Score";
			this->Width = 380;
			this->Height = 190;
			this->StartPosition = FormStartPosition::CenterParent;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->BackColor = Color::White;

			Label^ titleLabel = gcnew Label();
			titleLabel->Text = "New High Score!";
			titleLabel->Font = gcnew Drawing::Font("Segoe UI", 14, FontStyle::Bold);
			titleLabel->Location = Point(24, 18);
			titleLabel->Size = Drawing::Size(320, 28);

			Label^ promptLabel = gcnew Label();
			promptLabel->Text = "Enter your name:";
			promptLabel->Font = gcnew Drawing::Font("Segoe UI", 10);
			promptLabel->Location = Point(24, 55);
			promptLabel->Size = Drawing::Size(320, 22);

			nameTextBox = gcnew TextBox();
			nameTextBox->Text = "Player";
			nameTextBox->Font = gcnew Drawing::Font("Segoe UI", 10);
			nameTextBox->Location = Point(24, 80);
			nameTextBox->Size = Drawing::Size(320, 26);
			nameTextBox->SelectAll();

			saveButton = gcnew Button();
			saveButton->Text = "Save Score";
			saveButton->Location = Point(150, 115);
			saveButton->Size = Drawing::Size(95, 32);
			saveButton->DialogResult = System::Windows::Forms::DialogResult::OK;

			cancelButton = gcnew Button();
			cancelButton->Text = "Cancel";
			cancelButton->Location = Point(252, 115);
			cancelButton->Size = Drawing::Size(92, 32);
			cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;

			this->Controls->Add(titleLabel);
			this->Controls->Add(promptLabel);
			this->Controls->Add(nameTextBox);
			this->Controls->Add(saveButton);
			this->Controls->Add(cancelButton);

			this->AcceptButton = saveButton;
			this->CancelButton = cancelButton;
		}

		String^ GetPlayerName()
		{
			return nameTextBox->Text;
		}
	};
}