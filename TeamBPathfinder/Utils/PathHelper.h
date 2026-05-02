#pragma once

#include <string>

namespace Utils
{
	class PathHelper
	{
	public:
		static std::string getPuzzleFilePath();

		static std::string getSaveFilePath();
	};
}
