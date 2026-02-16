#pragma once
#include <chrono>
#include <vector>
#include "connection/Endpoint.h"
#include "MakeException.h"

MAKE_EXCEPTION(check_chain_speed, std::exception);

namespace utils
{
	struct CheckChainResult
	{
		std::chrono::steady_clock::duration time_connect;
		std::chrono::steady_clock::duration time_answer;
		connection::Ip external_ip;
	};

	CheckChainResult check_chain_speed(std::vector<connection::Endpoint> chain);
}
