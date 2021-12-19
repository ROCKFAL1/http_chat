#include "get.hpp"
#include "../tables/conversation_table.hpp"
#include "../tables/users_table.hpp"
#include "request_handler.hpp"
#include "constants.hpp"
#include "sessions.hpp"

pplx::task<void> server::get::conversations(server::request_handler* req_handler, web::http::http_request const& message)
{
	const auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

	if (paths.size() > 2)
	{
		return message.reply(web::http::status_codes::BadRequest);
	}

	const auto header = std::find_if(message.headers().begin(), message.headers().end(),
		[](std::pair<const utility::string_t, utility::string_t> header) { return header.first == body::SESSION_ID; });

	if (header == message.headers().end())
	{
		return message.reply(web::http::status_codes::Forbidden);
	}

	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;

	const std::string session_id = converter.to_bytes(header->second);
	const auto login_opt = server::sessions::get().get_login_by_id(session_id);

	if (login_opt == std::nullopt)
	{
		return message.reply(web::http::status_codes::Forbidden);
	}

	std::string const& login = login_opt.value();

	auto& users_table = req_handler->users_table;
	auto user_row_variant = users_table->get_by_login(login);

	if (user_row_variant.index() == 1)
	{
		return message.reply(web::http::status_codes::InternalError);
	}

	const auto user_row = std::get<pqxx::row>(user_row_variant);
	
	if (paths.size() == 1)
	{
		web::json::value result = web::json::value::array();
		const std::vector<std::uint32_t> friends_ids = users_table->get_frields_id_list(user_row);
		size_t counter = 0;

		if (friends_ids.size() == 0)
		{
			web::json::value result;
			result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("You don't have conversations!"));
			return message.reply(web::http::status_codes::OK, result);
		}

		for (std::uint32_t friend_id : friends_ids)
		{
			const auto friend_row_var = users_table->get_by_id(friend_id);

			if (friend_row_var.index() == 1)
			{
				continue;
			}

			const auto friend_row = std::get<pqxx::row>(friend_row_var);
			result[counter++] = web::json::value(converter.from_bytes(friend_row.at(1).as<std::string>()));
			
		}
		return message.reply(web::http::status_codes::OK, result);
	}

	const auto& with_whom = paths[1];
	const auto with_whow_row_variant = req_handler->users_table.get()->get_by_login(converter.to_bytes(with_whom));

	if (with_whow_row_variant.index() == 1)
	{
		web::json::value result;
		result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("There is no such user!"));
		return message.reply(web::http::status_codes::BadRequest, result);
	}

	auto with_whow_row = std::get<pqxx::row>(with_whow_row_variant);

	auto conv_table = table::conversation(config::get_conn_string(), login, converter.to_bytes(with_whom));
	const auto mes_history_var = conv_table.get_message_history();

	if (mes_history_var.index() == 1)
	{
		return message.reply(web::http::status_codes::InternalError);
	}

	const auto mes_history = std::get<std::vector<table::message>>(mes_history_var);

	web::json::value result = web::json::value::array();

	size_t counter = 0;

	for (const auto& mes : mes_history)
	{
		result[counter][body::SENDER] = web::json::value::string(converter.from_bytes(mes.sender));
		result[counter][body::MESSAGE] = web::json::value::string(converter.from_bytes(mes.data));
		counter++;
	}

	return message.reply(web::http::status_codes::OK, result);
}
