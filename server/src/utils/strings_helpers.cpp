#include "strings_helpers.hpp"

std::string utils::compare_str_by_chars(std::string const& s1, std::string const& s2)
{
	for (int i = 0; i < s1.size() && s2.size(); i++)
	{
		if (s1[i] < s2[i])
		{
			return s1;
		}
		else if (s1[i] > s2[i])
		{
			return s2;
		}
	}

	if (s1.size() < s2.size())
	{
		return s1;
	}

	return s2;
}
