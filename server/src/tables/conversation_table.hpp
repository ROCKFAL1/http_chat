#pragma once

#include <string>

#include "table_base.hpp"
#include "error.hpp"

namespace table
{
	struct message
	{
		std::string sender;
		std::string data;

		message() {}

		message(std::string const& sender, std::string const& data)
			: sender(sender), data(data)
		{

		}
	};

	class conversation final : public table_base
	{

	public:

		conversation(std::string connection_string,
			std::string const& interlocutor_1,
			std::string const& interlocutor_2);

		bool try_create_table();

		std::variant<bool, table::Error> add(std::string const& login_from, std::string const& msg);
		std::variant<std::vector<message>, table::Error> get_message_history();

	private:
		std::string interlocutor_1_;
		std::string interlocutor_2_;

		std::string table_name_;

	};

} //namespace table
