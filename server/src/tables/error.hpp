#pragma once

namespace table
{
	enum class Error
	{
		ConnectionError = 0,
		RequestError = 1,
		AlreadyExist = 2,
		NotFound = 3
	};
} //namespace table