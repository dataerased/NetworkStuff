#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include "TcpSocket.h"
#include "Endpoint.h"
#include "utils/NoCopy.h"
#include "utils/NoMove.h"

namespace connection
{
	class TcpListener : utils::NoCopy, utils::NoMove
	{
	public:
		TcpListener(SOCKET socket, const Endpoint& ep);
		~TcpListener();
		void close();
		Endpoint endpoint() const;
		TcpSocket accept();
		void async_accept(std::function<void(TcpSocket&& s)> cb);

	private:
		SOCKET m_socket;
		Endpoint m_ep;
		std::thread m_loop;
		std::atomic_bool m_running;

	};
}