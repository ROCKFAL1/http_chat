#include "json_helpers.hpp"

std::vector<std::uint32_t> utils::get_array_from_jsonparser(pqxx::array_parser& arr_parser)
{
	std::vector<std::uint32_t> result;
	std::pair<pqxx::array_parser::juncture, std::string> elem;

	do
	{
		elem = arr_parser.get_next();
		if (elem.first == pqxx::array_parser::juncture::string_value)
			result.push_back(std::stoi(elem.second));
	} while (elem.first != pqxx::array_parser::juncture::done);

	return result;
}
