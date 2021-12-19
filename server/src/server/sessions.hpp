#pragma once

#include <map>
#include <string>
#include <optional>
#include <mutex>

namespace server
{
	class sessions
	{

	public:

		static sessions& get()
		{
			static sessions instance;

			return instance;
		}
		
		sessions(sessions const&) = delete;
		void operator=(sessions const&) = delete;

		std::string add(std::string const& login);
		bool remove_by_login(std::string const& login);
		bool remove_by_id(std::string const& session_id);
		bool is_valid(std::string const& session_id) const;
		
		std::optional<std::string> get_id_by_login(std::string const& login) const;		
		std::optional<std::string> get_login_by_id(std::string const& session_id) const;
		
	private:

		std::map<std::string, std::string> active_sessions_; //key - session_id, value - user login
		mutable std::mutex mutex_;
		 
		sessions() {};

	};
} //namespace server
