#pragma once

#include <functional>

#include <cpprest/asyncrt_utils.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>

#include "config.hpp"
#include "../tables/users_table.hpp"

using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;

namespace server
{

	class request_handler
	{		
	public:

		request_handler(utility::string_t url);

		pplx::task<void> open() { return listener_.open(); }
		pplx::task<void> close() { return listener_.close(); }

		std::unique_ptr<table::users> users_table;

	private:

		pplx::task<void> handle_get(http_request message);
		pplx::task<void> handle_post(http_request message);

		http_listener listener_;

	};

} //namespace server