#include "SocketUtils.h"
#include "connection/Socks5.h"
#include "protocol/Http.h"

namespace utils
{
	CheckChainResult check_chain_speed(std::vector<connection::Endpoint> chain)
	{
		CheckChainResult result{};

		// 34.160.111.145:80 -> ifconfig.me
		chain.push_back({ "34.160.111.145", 80 });

		auto start = std::chrono::steady_clock::now();
		auto socket = connection::Socks5::connect(chain, std::chrono::seconds(20));
		result.time_connect = std::chrono::steady_clock::now() - start;

		protocol::Http http(std::move(socket), "ifconfig.me");

		start = std::chrono::steady_clock::now();
		auto answer = http.get();
		result.time_answer = std::chrono::steady_clock::now() - start;

		if (answer.code.find("200 OK") == std::string::npos)
			throw exception::check_chain_speed("check_chain_speed bad answer: " + answer.code);

		result.external_ip = answer.body;
		return result;
	}
}
