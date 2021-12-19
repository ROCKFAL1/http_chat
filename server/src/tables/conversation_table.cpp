#include "conversation_table.hpp"

#include "../utils/strings_helpers.hpp"

table::conversation::conversation(std::string connection_string,
											std::string const& interlocutor_1,
											std::string const& interlocutor_2)
	: table_base(connection_string)
{
	auto cmp = utils::compare_str_by_chars(interlocutor_1, interlocutor_2);

	interlocutor_1_ = cmp;
	interlocutor_2_ = cmp == interlocutor_1 ? interlocutor_2 : interlocutor_1;

	table_name_ = interlocutor_1_ + "_" + interlocutor_2_;

	try_connect();
	try_create_table();
}

bool table::conversation::try_create_table()
{

	try
	{
		std::string query = "CREATE TABLE IF NOT EXISTS ";
		query += table_name_;
		query += " ( \
				 id SERIAL PRIMARY KEY, \
				 sender TEXT NOT NULL, \
				 msg_text TEXT NOT NULL \
			   )";

		pqxx::work worker(*connection);

		worker.exec(query);
		worker.commit();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

std::variant<bool, table::Error> table::conversation::add(std::string const& login_from, std::string const& msg)
{
	if (!connection.has_value() || !try_connect())
	{
		return table::Error::ConnectionError;
	}

	if (!is_connected())
	{
		return table::Error::ConnectionError;
	}

	std::string query = "INSERT INTO ";
	query += table_name_;
	query += " (sender, msg_text)";
	query += " VALUES (";
	query += "'" + login_from + "'";
	query += " , ";
	query += "'" + msg + "'";
	query += " ) ";

	pqxx::work worker(*connection);
	worker.exec(query);
	worker.commit();

	return true;
}

std::variant<std::vector<table::message>, table::Error> table::conversation::get_message_history()
{
	if (!connection.has_value() || !try_connect())
	{
		return table::Error::ConnectionError;
	}

	if (!is_connected())
	{
		return table::Error::ConnectionError;
	}

	std::string query = "SELECT sender, msg_text FROM " + table_name_;

	pqxx::work worker(*connection);
	pqxx::result result = worker.exec(query);
	std::vector<table::message> vector_result;
	
	for (auto row : result)
	{
		vector_result.emplace_back(row[0].as<std::string>(), row[1].as<std::string>());
	}

	return vector_result;
}
