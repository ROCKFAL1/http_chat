#include "sessions.hpp"
#include "../utils/generator.hpp"

std::string server::sessions::add(std::string const& login)
{
	std::lock_guard<std::mutex> lg(mutex_);
	const std::string session_id = utils::generate_session_id();
	active_sessions_[session_id] = login;
	return session_id;
}

bool server::sessions::remove_by_login(std::string const& login)
{
	std::lock_guard<std::mutex> lg(mutex_);

	for (auto& session : active_sessions_)
	{
		if (session.second == login)
		{
			active_sessions_.erase(session.first);
			return true;
		}
	}

	return false;
}

bool server::sessions::remove_by_id(std::string const& session_id)
{
	std::lock_guard<std::mutex> lg(mutex_);

	if (!active_sessions_.contains(session_id))
	{
		return false;
	}

	active_sessions_.erase(session_id);
	return true;
}

bool server::sessions::is_valid(std::string const& session_id) const
{
	std::lock_guard<std::mutex> lg(mutex_);
	return active_sessions_.contains(session_id);
}

std::optional<std::string> server::sessions::get_id_by_login(std::string const& login) const
{
	std::lock_guard<std::mutex> lg(mutex_);

	for (auto& session : active_sessions_)
	{
		if (session.second == login)
		{
			return session.first;
		}
	}

	return std::nullopt;
}

std::optional<std::string> server::sessions::get_login_by_id(std::string const& session_id) const
{
	std::lock_guard<std::mutex> lg(mutex_);
	if (!active_sessions_.contains(session_id))
	{
		return std::nullopt;
	}

	return active_sessions_.at(session_id);
}
