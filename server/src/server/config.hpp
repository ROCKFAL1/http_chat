#pragma once

#include <string>

namespace server
{
	namespace config
	{
		void set_conn_string(std::string const& string);
		std::string get_conn_string();
	} //namespace config

} //namespace server


