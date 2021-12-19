#pragma once

#include <cpprest/asyncrt_utils.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>

namespace server
{
	class request_handler;

	namespace post
	{
		pplx::task<void> registration(server::request_handler* req_handler, web::http::http_request const& message);
		pplx::task<void> login(server::request_handler* req_handler, web::http::http_request const& message);
		pplx::task<void> conversations(server::request_handler* req_handler, web::http::http_request const& message);

		namespace PRIV
		{
			pplx::task<void> send_message(server::request_handler* req_handler, web::http::http_request const& message);
			pplx::task<void> add_conversation(server::request_handler* req_handler, web::http::http_request const& message);
		}
	}
} //namespace server

