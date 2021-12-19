#include "post.hpp"
#include "../tables/conversation_table.hpp"
#include "../tables/users_table.hpp"
#include "request_handler.hpp"
#include "constants.hpp"
#include "sessions.hpp"

pplx::task<void> server::post::registration(server::request_handler* req_handler, web::http::http_request const& message)
{
	auto request_body = message.extract_json().get();

	if (!request_body.has_field(body::LOGIN) || !request_body.has_field(body::PASSWORD))
	{
		return message.reply(web::http::status_codes::BadRequest);
	}

	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
	const std::string login = converter.to_bytes(request_body[body::LOGIN].as_string());
	const auto exist_variant = req_handler->users_table.get()->is_exist(login);

	if (exist_variant.index() == 1)
	{
		return message.reply(web::http::status_codes::InternalError);
	}

	if (std::get<bool>(exist_variant))
	{
		web::json::value result;
		result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("User with this login already exist!"));
		return message.reply(web::http::status_codes::OK, result);
	}

	const std::string password = converter.to_bytes(request_body[body::PASSWORD].as_string());
	const auto add_variant = req_handler->users_table.get()->add(table::user(login, password));

	if (add_variant.index() == 1)
	{
		return message.reply(web::http::status_codes::InternalError);
	}

	web::json::value result;
	result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("Successfully registered!"));
	return message.reply(web::http::status_codes::OK, result);
}

pplx::task<void> server::post::login(server::request_handler* req_handler, web::http::http_request const& message)
{
	auto request_body = message.extract_json().get();

	if (!request_body.has_field(body::LOGIN) || !request_body.has_field(body::PASSWORD))
	{
		return message.reply(web::http::status_codes::BadRequest);
	}

	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
	const std::string login = converter.to_bytes(request_body[body::LOGIN].as_string());
	const auto exist_variant = req_handler->users_table.get()->is_exist(login);

	if (exist_variant.index() == 1)
	{		
		return message.reply(web::http::status_codes::InternalError);
	}

	if (!std::get<bool>(exist_variant))
	{
		web::json::value result;
		result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("There is no user with this username or password!"));	
		return message.reply(web::http::status_codes::OK, result);
	}

	const std::string password = converter.to_bytes(request_body[body::PASSWORD].as_string());
	const auto password_valid_variant = req_handler->users_table->is_password_valid(login, password);

	if (password_valid_variant.index() == 1)
	{	
		return message.reply(web::http::status_codes::InternalError);
	}

	const auto id_by_login = server::sessions::get().get_id_by_login(login);
	if (id_by_login != std::nullopt)
	{
		web::json::value result;
		result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("You are already logged in!"));
		result[body::SESSION_ID] = web::json::value::string(converter.from_bytes(id_by_login.value()));	
		return message.reply(web::http::status_codes::OK, result);
	}

	if (!std::get<bool>(password_valid_variant))
	{
		web::json::value result;
		result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("Incorrect password!"));		
		return message.reply(web::http::status_codes::OK, result);
	}

	web::json::value result;
	result[body::MESSAGE_FOR_CLIENT] = web::json::value::string(U("Successfully logged in!"));
	result[body::SESSION_ID] = web::json::value::string(converter.from_bytes(server::sessions::get().add(login)));
	
	return message.reply(web::http::status_codes::OK, result);
}

pplx::task<void> server::post::PRIV::send_message(server::request_handler * req_handler, web::http::http_request const& message)
{
	const auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

	if (paths.size() != 2)
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

	const auto sender_login = login_opt.value();
	const auto recipient_login_w = paths[1];

	const auto is_recipient_exist = req_handler->users_table->is_exist(converter.to_bytes(recipient_login_w));

	if (is_recipient_exist.index() == 1)
	{
		return message.reply(web::http::status_codes::InternalError);
	}

	if (!std::get<bool>(is_recipient_exist))
	{
		return message.reply(web::http::status_codes::NotFound);
	}

	const auto recipient_login = converter.to_bytes(recipient_login_w);

	auto conv_table = table::conversation(config::get_conn_string(), sender_login, recipient_login);

	conv_table.add(sender_login, converter.to_bytes(message.extract_json().get().at(body::MESSAGE).as_string()));
	return message.reply(web::http::status_codes::OK);

}

pplx::task<void> server::post::PRIV::add_conversation(server::request_handler* req_handler, web::http::http_request const& message)
{
	const auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

	if (paths.size() != 3)
	{
		return message.reply(web::http::status_codes::BadRequest);
	}

	const auto id_header = std::find_if(message.headers().begin(), message.headers().end(),
		[](std::pair<const utility::string_t, utility::string_t> header) { return header.first == body::SESSION_ID; });

	if (id_header == message.headers().end())
	{
		return message.reply(web::http::status_codes::Forbidden);
	}

	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
	const std::string session_id = converter.to_bytes(id_header->second);
	const auto login_opt = server::sessions::get().get_login_by_id(session_id);

	if (login_opt == std::nullopt)
	{
		return message.reply(web::http::status_codes::Forbidden);
	}

	if (paths[1] != req::conversations::ADD)
	{
		return message.reply(web::http::status_codes::BadRequest);
	}

	const auto sender_login_w = login_opt.value();
	const auto recipient_login_w = paths[2];

	const auto is_recipient_exist = req_handler->users_table->is_exist(converter.to_bytes(recipient_login_w));

	if (is_recipient_exist.index() == 1)
	{
		return message.reply(web::http::status_codes::InternalError);
	}

	if (!std::get<bool>(is_recipient_exist))
	{
		return message.reply(web::http::status_codes::NotFound);
	}

	const auto sender_login = sender_login_w;
	const auto recipient_login = converter.to_bytes(recipient_login_w);

	req_handler->users_table->add_conversation(sender_login, recipient_login);
	return message.reply(web::http::status_codes::OK);
}

pplx::task<void> server::post::conversations(server::request_handler* req_handler, web::http::http_request const& message)
{
	const auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

	if (paths.size() == 2)
	{
		return PRIV::send_message(req_handler, message);
	}
	else if (paths.size() == 3)
	{
		const auto type = paths[1];

		if (type == req::conversations::ADD)
		{
			return PRIV::add_conversation(req_handler, message);
		}

	}

	return message.reply(web::http::status_codes::BadRequest);
}
