#include "PathHelper.h"

#include <msclr/marshal_cppstd.h>

namespace Utils
{
	std::string PathHelper::getPuzzleFilePath()
	{
		System::String^ pathManaged = System::Windows::Forms::Application::StartupPath + "\\puzzles.txt";
		return msclr::interop::marshal_as<std::string>(pathManaged);
	}

	std::string PathHelper::getSaveFilePath()
	{
		System::String^ pathManaged = System::Windows::Forms::Application::StartupPath + "\\savegame.txt";

		return msclr::interop::marshal_as<std::string>(pathManaged);
	}
}
