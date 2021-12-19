#include "table_base.hpp"

table::table_base::table_base(std::string connection_string)
{
	connection_string_ = connection_string;
}

bool table::table_base::try_connect()
{
	try
	{
		connection = pqxx::connection(connection_string_);
	}
	catch(...)
	{
		return false;
	}

	if (!connection.has_value())
	{
		return false;
	}

	return connection->is_open();
}

bool table::table_base::is_connected() const
{
	if (!connection.has_value())
	{
		return false;
	}

	return connection->is_open();
}