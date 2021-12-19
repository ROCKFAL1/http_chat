#pragma once

#include <string>
#include <random>

namespace utils
{
	constexpr std::string numbers();
	
	constexpr std::string latin_alphabet();
	 
	std::string generate_session_id(size_t length = 32);

} //namespace utils