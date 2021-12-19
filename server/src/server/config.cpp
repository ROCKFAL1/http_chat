#include "config.hpp"

std::string conn_string;

void server::config::set_conn_string(std::string const& string)
{
	conn_string = string;
}

std::string server::config::get_conn_string()
{
	return conn_string;
}
