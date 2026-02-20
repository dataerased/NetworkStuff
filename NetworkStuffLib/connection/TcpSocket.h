#pragma once
#include <mutex>
#include <thread>
#include <functional>
#include <vector>
#include "Endpoint.h"
#include "utils/NoCopy.h"
#include "utils/MakeException.h"

MAKE_EXCEPTION(tcp_socket, std::exception);

namespace connection
{
	class TcpSocket : public utils::NoCopy
	{
	public:
		TcpSocket(SOCKET socket, const Endpoint &ep);
		~TcpSocket();
		TcpSocket(TcpSocket&& o) noexcept;
		TcpSocket& operator=(TcpSocket&& o) noexcept;

		void close();
		int write(const void* data, int size);
		int read(void* data, int size, const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(1));
		void async_read(size_t buffer_max_size, std::function<void(const std::vector<char>& buffer)> cb);
		void stop_async();
		operator bool() const;
		const Endpoint& endpoint() const;

	private:
		SOCKET m_socket;
		Endpoint m_ep;
		std::thread m_read_loop;
		std::atomic_bool m_reading;

	};
}
