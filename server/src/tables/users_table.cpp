#include "users_table.hpp"
#include "../utils/json_helpers.hpp"
#include "../tables/conversation_table.hpp"

table::users::users(std::string connection_string)
	: table_base(connection_string)
{

	try_connect();
	try_create_table();
}

bool table::users::try_create_table()
{
	try
	{
		std::string query = "CREATE TABLE IF NOT EXISTS ";
		query += table_name_;
		query += " ( id SERIAL PRIMARY KEY, \
					login TEXT NOT NULL UNIQUE, \
					password_hash TEXT NOT NULL, \
					conversations INTEGER[] )";

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

std::variant<pqxx::row, table::Error> table::users::get_by_login(std::string const& login)
{
	if (!connection.has_value() || !try_connect())
	{
		return table::Error::ConnectionError;
	}

	if (!is_connected())
	{
		return table::Error::ConnectionError;
	}

	std::string query = "SELECT * FROM ";
	query += table_name_;
	query += " WHERE login = ";
	query += "'" + login + "'";

	pqxx::work worker(*connection);
	pqxx::result result = worker.exec(query);

	if (result.affected_rows() == 0)
	{
		return table::Error::NotFound;
	}
	 
	return result.at(0);
}

std::vector<std::uint32_t> table::users::get_frields_id_list(pqxx::row const& row)
{
	auto arr_parser = row.at(3).as_array();
	return utils::get_array_from_jsonparser(arr_parser);
}

std::variant<bool, table::Error> table::users::add_conversation(std::string const& interlocutor_1, std::string const& interlocutor_2)
{
	auto const inter1_row_variant = get_by_login(interlocutor_1);
	auto const inter2_row_variant = get_by_login(interlocutor_2);

	if (inter1_row_variant.index() == 1 || inter2_row_variant.index() == 1)
	{
		return table::Error::RequestError;
	}

	auto inter1_row = std::get<pqxx::row>(inter1_row_variant);
	auto inter2_row = std::get<pqxx::row>(inter2_row_variant);

	auto inter1_convs_parser_arr = inter1_row.at(3).as_array();
	auto inter2_convs_parser_arr = inter2_row.at(3).as_array();

	auto inter1_convs = utils::get_array_from_jsonparser(inter1_convs_parser_arr);
	auto inter2_convs = utils::get_array_from_jsonparser(inter2_convs_parser_arr);

	const auto is_vector_contains = [](std::vector<std::uint32_t> vec, const std::uint32_t val)
	{
		auto const iter = std::find(vec.begin(), vec.end(), val);
		return iter != vec.end();
	};

	auto inter1_id = inter1_row.at(0).as<std::uint32_t>();
	auto inter2_id = inter2_row.at(0).as<std::uint32_t>();

	auto update_query = [&](std::uint32_t user_id, std::vector<std::uint32_t> new_conv_list)
	{
		const size_t arr_size = new_conv_list.size();
		std::string arr = " { ";
		for (size_t i = 0; i < arr_size; i++)
		{
			arr += std::to_string(new_conv_list.at(i));
			if (i + 1 < arr_size)
			{
				arr += " , ";
			}

		}
		arr += " } ";

		std::string result = "UPDATE " + table_name_;
		result += " SET conversations = '" + arr + "'";
		result += " WHERE id = " + std::to_string(user_id);
		return result;
	};

	pqxx::work worker(*connection);
	
	if (!is_vector_contains(inter1_convs, inter2_id))
	{
		inter1_convs.push_back(inter2_id);
		worker.exec(update_query(inter1_id, inter1_convs));
	}

	if (!is_vector_contains(inter2_convs, inter1_id))
	{
		inter2_convs.push_back(inter1_id);
		worker.exec(update_query(inter2_id, inter2_convs));
	}

	worker.commit();

	return true;
}

std::variant<bool, table::Error> table::users::add(table::user const& user)
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
	query += "(login, password_hash)";
	query += " VALUES (";
	query += "'" + user.login + "'";
	query += " , ";
	query += "'" + user.get_password_hash() + "'";
	query += " ) ";

	pqxx::work worker(*connection);
	worker.exec(query);
	worker.commit();

	return true;
}

std::variant<bool, table::Error> table::users::remove(table::user const& user)
{
	if (!connection.has_value() || !try_connect())
	{
		return table::Error::ConnectionError;
	}

	if (!is_connected())
	{
		return table::Error::ConnectionError;
	}

	std::string query = "DELETE FROM ";
	query += table_name_;
	query += " WHERE login = ";
	query += "'" + user.login + "'";

	pqxx::work worker(*connection);
	const pqxx::result result = worker.exec(query);
	worker.commit();

	if (result.affected_rows() == 0)
	{
		return table::Error::NotFound;
	}

	return true;
}

std::variant<pqxx::row, table::Error> table::users::get_by_id(std::uint32_t id)
{
	if (!connection.has_value() || !try_connect())
	{
		return table::Error::ConnectionError;
	}

	if (!is_connected())
	{
		return table::Error::ConnectionError;
	}

	std::string query = "SELECT * FROM ";
	query += table_name_;
	query += " WHERE id = ";
	query += std::to_string(id);

	pqxx::work worker(*connection);
	pqxx::result result = worker.exec(query);

	if (result.affected_rows() == 0)
	{
		return table::Error::NotFound;
	}

	return result.at(0);
}

std::variant<bool, table::Error> table::users::is_password_valid(std::string const& login, std::string const& test_password)
{
	if (!connection.has_value() || !try_connect())
	{
		return table::Error::ConnectionError;
	}

	if (!is_connected())
	{
		return table::Error::ConnectionError;
	}

	const auto row_variant = get_by_login(login);
	pqxx::row row;

	if (row_variant.index() == 1)
	{
		return std::get<table::Error>(row_variant);
	}
	else
	{
		row = std::get<pqxx::row>(row_variant);
	}

	const std::string user_password_hash = row.at(2).as<std::string>();

	return user_password_hash == pqxx::encrypt_password(login, test_password);
}

std::variant<bool, table::Error> table::users::is_exist(std::string const& login)
{
	if (!connection.has_value() || !try_connect())
	{
		return table::Error::ConnectionError;
	}

	if (!is_connected())
	{
		return table::Error::ConnectionError;
	}

	const auto row_variant = get_by_login(login);
	
	if (row_variant.index() == 1)
	{
		const auto err = std::get<table::Error>(row_variant);
		if (err == table::Error::NotFound)
		{
			return false;
		}
		else
		{
			return err;
		}
	}

	return true;	
}