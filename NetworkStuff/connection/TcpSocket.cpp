#include "TcpSocket.h"
#include <vector>

namespace connection
{
	TcpSocket::TcpSocket(SOCKET socket, const Endpoint &ep)
		: m_socket(socket)
		, m_ep(ep)
	{
		if (*this)
		{
			DWORD timeout = 10;
			if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0
				|| setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
			{
				throw exception::tcp_socket("failed to set timeouts");
			}

			int flag = 1;
			if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
			{
				throw exception::tcp_socket("failed to set nodelay flag");
			}
		}
	}

	TcpSocket::~TcpSocket()
	{
		close();
	}

	TcpSocket::TcpSocket(TcpSocket&& o) noexcept
		: m_socket(o.m_socket)
		, m_ep(o.m_ep)
	{
		o.m_socket = INVALID_SOCKET;
		o.m_ep = {};
	}

	TcpSocket& TcpSocket::operator=(TcpSocket&& o) noexcept
	{
		close();
		if (this != &o) {

			m_socket = o.m_socket;
			m_ep = o.m_ep;

			o.m_socket = INVALID_SOCKET;
			o.m_ep = {};
		}
		return *this;
	}

	TcpSocket::operator bool() const
	{
		return m_socket != INVALID_SOCKET;
	}

	void TcpSocket::close()
	{
		stop_async();
		if (m_socket != INVALID_SOCKET)
		{
			shutdown(m_socket, SD_BOTH);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
	}
	
	const Endpoint& TcpSocket::endpoint() const
	{
		return m_ep;
	}

	int TcpSocket::write(const void* p_data, int size)
	{
		auto data = static_cast<const char*>(p_data);
		auto elapsed = size;
		while (elapsed > 0)
		{
			auto sended = ::send(m_socket, data, elapsed, NULL);

			if (sended == 0)
			{
				close();
				break;
			}
			else if (sended == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSAETIMEDOUT)
					break;

				throw exception::tcp_socket("send error");
			}

			data += sended;
			elapsed -= sended;
		}
		return size - elapsed;
	}

	int TcpSocket::read(void* p_data, int size, const std::chrono::steady_clock::duration& timeout)
	{
		auto data = static_cast<char*>(p_data);
		auto elapsed = size;
		auto deadline = std::chrono::steady_clock::now() + timeout;

		while (elapsed > 0 && std::chrono::steady_clock::now() <= deadline)
		{
			auto recieved = ::recv(m_socket, data, elapsed, NULL);

			if (recieved == 0)
			{
				close();
				break;
			}
			else if (recieved == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSAETIMEDOUT)
				{
					continue;
				}
				else
				{
					throw exception::tcp_socket("recv error");
				}
			}

			data += recieved;
			elapsed -= recieved;
		}
		return size - elapsed;
	}

	void TcpSocket::async_read(size_t buffer_max_size, std::function<void(const char* data, size_t size)> cb)
	{
		m_reading = true;
		m_read_loop = std::thread([&, buffer_max_size, cb]() {
			std::vector<char> buffer(buffer_max_size, 0);
			while (m_reading && static_cast<bool>(*this))
			{
				try
				{
					auto readed = read(&buffer[0], static_cast<int>(buffer.size()), std::chrono::milliseconds(100));
					if (readed)
						cb(buffer.data(), readed);
				}
				catch (const std::exception&)
				{
					if (m_reading)
						throw;
				}
			}
		});
	}

	void TcpSocket::stop_async()
	{
		m_reading = false;
		if (m_read_loop.joinable())
			m_read_loop.join();
	}
}