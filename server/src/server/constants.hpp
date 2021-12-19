#pragma once

namespace body
{
	constexpr auto MESSAGE_FOR_CLIENT = U("message_for_client");
	constexpr auto SESSION_ID = U("session_id");
	constexpr auto MESSAGE = U("message");
	constexpr auto SENDER = U("sender");
	constexpr auto LOGIN = U("login");
	constexpr auto PASSWORD = U("password");

} //namespace body

namespace req
{
	constexpr auto LOGIN = U("login");
	constexpr auto REGISTRATION = U("registration");

	constexpr auto CONVERSATIONS = U("conversations");
	namespace conversations
	{
		constexpr auto ADD = U("add");
	} //namespace conversations

} //namespace req
