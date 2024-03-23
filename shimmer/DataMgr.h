#pragma once
#ifndef _CDataMgr_
#define _CDataMgr_
#include <string>
#include <map>
#include <deque>

struct server_list_t {
	std::string ip;
	std::string city;
	std::string country;
};

#include <rpc/client.h>
#include <rpc/rpc_error.h>

class CDataMgr{
public:
	virtual void update();
	void pull();

	//rpc client
	template <typename... Args>
	static std::string rpcjson_dispath(std::string const &func_name, Args... args) {
		rpc::client client("127.0.0.1", rpc::constants::DEFAULT_PORT);
		try {
			//client.wait_conn();
			client.wait_all_responses();
			rpc::client::connection_state state = client.get_connection_state();
			if (state == rpc::client::connection_state::connected) {
				auto ft_long = client.async_call(func_name, args...);
				ft_long.wait();
				return ft_long.get().as<std::string>();
			}
		}
		catch (rpc::system_error &e) {
			std::cout << "system code:" << e.code() << "what:" << e.what() << std::endl;
		}
		return "{}";
	}

	std::deque<server_list_t> get_serer_list();

private:
	std::deque<server_list_t> server_list;
};

#endif