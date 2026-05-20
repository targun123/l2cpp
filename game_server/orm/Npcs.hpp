#pragma once

// C++ includes
#include <unordered_map>
#include <string>
#include <l2cpp/Typedefs.hpp>

namespace Orm
{
	void loadNpcHtmls(std::unordered_map<u64, std::wstring>&);
}