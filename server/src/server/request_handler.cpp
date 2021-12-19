#include "request_handler.hpp"

#include "get.hpp"
#include "post.hpp"
#include "constants.hpp"
#include "../tables/conversation_table.hpp"


server::request_handler::request_handler(utility::string_t url)
	: listener_(url)
{
	users_table = std::make_unique<table::users>(config::get_conn_string());

	listener_.support(methods::GET, std::bind(&server::request_handler::handle_get, this, std::placeholders::_1));
	listener_.support(methods::POST, std::bind(&server::request_handler::handle_post, this, std::placeholders::_1));
}

pplx::task<void> server::request_handler::handle_get(http_request message)
{
	auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

	if (paths.empty())
	{
		return message.reply(web::http::status_codes::OK);
	}

	auto str = paths[0];
	
	if (str == req::CONVERSATIONS)
	{
		return server::get::conversations(this, message);
	}

	return message.reply(web::http::status_codes::BadRequest);
}

pplx::task<void> server::request_handler::handle_post(http_request message)
{
	auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

	ucout << message.to_string() << std::endl;

	if (paths.empty())
	{	
		return message.reply(web::http::status_codes::OK);
	}

	auto str = paths[0];

	if (str == req::LOGIN)
	{
		return server::post::login(this, message);
	}
	else if (str == req::REGISTRATION)
	{
		return server::post::registration(this, message);
	}
	else if (str == req::CONVERSATIONS)
	{
		return server::post::conversations(this, message);
	}

	return message.reply(web::http::status_codes::BadRequest);
}