#include "SocketFactory.h"

namespace connection
{
	SocketFactory& SocketFactory::get()
	{
		static SocketFactory instance{};
		return instance;
	}

	SocketFactory::SocketFactory()
	{
		WSADATA data{};
		auto version = MAKEWORD(2, 2);
		auto err = WSAStartup(version, &data);
		if (err)
		{
			throw exception::factory("WSAStartup error");
		}
	}

	SocketFactory::~SocketFactory()
	{
		WSACleanup();
	}

	TcpSocket SocketFactory::connect(const Endpoint& ep)
	{
		SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == INVALID_SOCKET)
		{
			throw exception::factory("socket error");
		}

		auto addr = ep.sockaddr();
		auto res = ::connect(s, (SOCKADDR*)&addr, sizeof(addr));
		if (res == SOCKET_ERROR)
		{
			throw exception::factory("connect error");
		}

		return { s, ep };
	}

	TcpListener SocketFactory::listen(uint16_t port)
	{
		return listen({ "0.0.0.0", port });
	}

	TcpListener SocketFactory::listen(const Endpoint& ep)
	{
		SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in service{};
		service.sin_family = AF_INET;
		service.sin_port = ep.network_port();
		service.sin_addr.S_un.S_addr = ep.ip().to_network();
		::bind(s, (sockaddr*)&service, sizeof(service));
		::listen(s, 0);

		return { s, ep };
	}


}
