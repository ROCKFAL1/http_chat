#pragma once

#include <pqxx/pqxx>

namespace utils
{
	std::vector<std::uint32_t> get_array_from_jsonparser(pqxx::array_parser& arr_parser);	
} //namespace utils
