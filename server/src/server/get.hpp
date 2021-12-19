#pragma once

#include <cpprest/asyncrt_utils.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>

namespace server
{
	class request_handler;

	namespace get
	{	
		pplx::task<void> conversations(server::request_handler* req_handler, web::http::http_request const& message);
	}
} //namespace server