#pragma once
#include <string>
#include <stdint.h>

namespace connection
{
	class Ip
	{
	public:
		Ip() = default;
		Ip(const std::string& ip);

		static Ip FromNetwork(uint32_t network);
		static Ip FromHost(uint32_t host);

		std::string to_string() const;
		uint32_t to_host() const;
		uint32_t to_network() const;

	private:
		Ip(uint32_t ip);
		uint32_t m_ip;

	};
}