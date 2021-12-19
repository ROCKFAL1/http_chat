#include "generator.hpp"

constexpr std::string utils::numbers()
{
	std::string result;
	result.resize(10);

	size_t counter = 0;

	for (char i = '0'; i <= '9'; ++i)
	{
		result[counter] = i;
		++counter;
	}

	return result;
}

constexpr std::string utils::latin_alphabet()
{
	std::string result;
	result.resize(52);

	size_t counter = 0;

	for (char i = 'A'; i <= 'Z'; ++i)
	{
		result[counter] = i;
		++counter;
	}

	for (char i = 'a'; i <= 'z'; ++i)
	{
		result[counter] = i;
		++counter;
	}

	return result;
}

std::string utils::generate_session_id(size_t length)
{
	const std::string alphabet = latin_alphabet() + numbers();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, alphabet.size() - 1);

	std::string session_id;
	session_id.resize(length);

	for (auto& ch : session_id)
	{
		ch = alphabet[dist(gen)];
	}

	return session_id;
}
