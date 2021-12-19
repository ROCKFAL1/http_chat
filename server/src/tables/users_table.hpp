#pragma once

#include <string>

#include <pqxx/pqxx>

#include "table_base.hpp"
#include "error.hpp"

namespace table
{
	class user
	{

	public:

		std::uint32_t id;
		std::string login;
		std::string password;
		std::vector<std::uint32_t> friends_id;

		user() {};

		user(std::string login, std::string password)
			: login(login), password(password)
		{

		}

		std::string get_password_hash() const
		{
			return pqxx::encrypt_password(login, password);
		}
	};

	class users final : public table_base
	{

	public:

		users(std::string connection_string);

		bool try_create_table();

		std::variant<pqxx::row, table::Error> get_by_id(std::uint32_t id);
		std::variant<pqxx::row, table::Error> get_by_login(std::string const& login);
		std::vector<std::uint32_t> get_frields_id_list(pqxx::row const& row);
		
		std::variant<bool, table::Error> add_conversation(std::string const& interlocutor_1, std::string const& interlocutor_2);
		std::variant<bool, table::Error> add(table::user const& user);
		std::variant<bool, table::Error> remove(table::user const& user);	
		std::variant<bool, table::Error> is_password_valid(std::string const& login, std::string const& test_password);
		std::variant<bool, table::Error> is_exist(std::string const& login);

	private:
		std::string table_name_ = "users";

	};

} //namespace table