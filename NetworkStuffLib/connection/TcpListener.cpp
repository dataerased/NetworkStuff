#include "TcpListener.h"

namespace connection
{
	TcpListener::TcpListener(SOCKET socket, const Endpoint& ep)
		: m_socket(socket)
		, m_ep(ep)
		, m_running(false)
	{
	}

	TcpListener::TcpListener(TcpListener&& o) noexcept
		: m_socket(o.m_socket)
		, m_ep(o.m_ep)
		, m_running(false)
	{
		m_socket = INVALID_SOCKET;
		m_ep = {};
	}

	TcpListener& TcpListener::operator=(TcpListener&& o) noexcept
	{
		if (this != &o) {

			close();
			m_socket = o.m_socket;
			m_ep = o.m_ep;

			o.m_socket = INVALID_SOCKET;
			o.m_ep = {};
		}
		return *this;
	}

	TcpListener::~TcpListener()
	{
		close();
	}

	void TcpListener::close()
	{
		if (m_socket != INVALID_SOCKET)
		{
			shutdown(m_socket, SD_BOTH);
			closesocket(m_socket);
		}

		m_running = false;
		if (m_loop.joinable())
			m_loop.join();
	}

	Endpoint TcpListener::endpoint() const
	{
		return m_ep;
	}

	TcpSocket TcpListener::accept()
	{
		SOCKADDR_IN client_addr{};
		int addr_len = sizeof(client_addr);
		SOCKET s = ::accept(m_socket, (SOCKADDR*)&client_addr, &addr_len);
		if (s == INVALID_SOCKET)
			return { s, {} };

		return { s, { inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port) } };
	}

	void TcpListener::async_accept(std::function<void(TcpSocket&& s)> cb)
	{
		m_running = true;
		m_loop = std::thread([&, cb]() {
			while (m_running && m_socket != INVALID_SOCKET)
			{
				auto sock = accept();
				if (sock)
					cb(std::move(sock));
			}
		});
	}
}