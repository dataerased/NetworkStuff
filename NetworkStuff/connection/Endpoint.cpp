#include "Endpoint.h"

namespace connection
{
	Endpoint::Endpoint(const std::string& ip, uint16_t port)
		: m_ip(ip)
		, m_port(htons(port))
	{
	}
	Endpoint::Endpoint(Ip ip, uint16_t port)
		: m_ip(ip)
		, m_port(htons(port))
	{
	}

	sockaddr_in Endpoint::sockaddr() const
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = m_ip.to_network();
		addr.sin_port = network_port();
		return addr;
	}

	std::string Endpoint::to_string() const
	{
		auto result = m_ip.to_string();
		result += ":" + std::to_string(port());
		return result;
	}

	Ip Endpoint::ip() const
	{
		return m_ip;
	}

	uint16_t Endpoint::port() const
	{
		return ntohs(m_port);
	}

	uint16_t Endpoint::network_port() const
	{
		return m_port;
	}
}