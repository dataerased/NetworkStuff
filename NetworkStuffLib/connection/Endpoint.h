#pragma once
#include "Ip.h"
#include <WinSock2.h>

namespace connection
{
	// Add Endpoint with dns
	class Endpoint
	{
	public:
		Endpoint() = default;
		Endpoint(const std::string& ip, uint16_t port);
		Endpoint(Ip ip, uint16_t port);
		sockaddr_in sockaddr() const;
		std::string to_string() const;
		Ip ip() const;
		uint16_t port() const;
		uint16_t network_port() const;

	private:
		Ip m_ip;
		uint16_t m_port;

	};
}