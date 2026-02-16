#include "Ip.h"
#include <WinSock2.h>

namespace connection
{
	Ip::Ip(const std::string& ip)
		: m_ip(inet_addr(ip.c_str()))
	{
	}

	Ip Ip::FromNetwork(uint32_t network)
	{
		return Ip(network);
	}

	Ip Ip::FromHost(uint32_t host)
	{
		return Ip(htonl(host));
	}

	std::string Ip::to_string() const
	{
		std::string ip;

		ip += std::to_string(m_ip & 0xFF) + '.';
		ip += std::to_string((m_ip >> 8) & 0xFF) + '.';
		ip += std::to_string((m_ip >> 16) & 0xFF) + '.';
		ip += std::to_string((m_ip >> 24) & 0xFF);

		return ip;
	}

	uint32_t Ip::to_host() const
	{
		return ntohl(m_ip);
	}

	uint32_t Ip::to_network() const
	{
		return m_ip;
	}

	Ip::Ip(uint32_t ip)
		: m_ip(ip)
	{
	}
}