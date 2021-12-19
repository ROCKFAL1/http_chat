#pragma once

#include <string>

#include <pqxx/pqxx>

namespace table
{
	class table_base
	{

	public:

		bool try_connect();
		bool is_connected() const;

		virtual bool try_create_table() = 0;

		std::optional<pqxx::connection> connection;

	protected:

		table_base(std::string connection_string);
		virtual ~table_base() {};
	
	private:

		std::string connection_string_;

	};
} //namespace table
