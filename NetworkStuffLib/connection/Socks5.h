#pragma once
#include "TcpSocket.h"
#include "utils/NoMove.h"
#include "utils/NoCopy.h"
#include "utils/MakeException.h"
#include <vector>
#include <chrono>

MAKE_EXCEPTION(socks5, std::exception);
MAKE_EXCEPTION(bad_endpoint, socks5);
MAKE_EXCEPTION(bad_chain, socks5);

namespace connection
{
	class Socks5 : utils::NoCopy, utils::NoMove
	{
	public:
		static TcpSocket connect(const std::vector<Endpoint>& chain, const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(30));
		static TcpSocket connect(const Endpoint& proxy, const Endpoint& ep, const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(1));

	private:
		Socks5() = default;
		~Socks5();
		
		static void connect(TcpSocket& socket, const Endpoint& ep, const std::chrono::steady_clock::duration& timeout);

		static void send_hello(TcpSocket& socket, const std::chrono::steady_clock::duration& timeout);
		static void send_request(TcpSocket& socket, const Endpoint& ep, const std::chrono::steady_clock::duration& timeout);
	};
}
