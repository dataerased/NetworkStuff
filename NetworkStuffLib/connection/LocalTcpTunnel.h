#pragma once
#include <functional>
#include <list>
#include "TcpSocket.h"
#include "TcpListener.h"
#include "utils/NoMove.h"
#include "utils/NoCopy.h"

namespace connection
{
	class LocalTcpTunnel
	{
	public:
		LocalTcpTunnel(const std::function<TcpSocket()> creator, uint16_t local_port, const Endpoint& ep);
		~LocalTcpTunnel();

	private:
		class Tunnel
		{
		public:
			Tunnel(TcpSocket&& s1, TcpSocket&& s2, const std::string& replace_from, const std::string& replace_to);
			~Tunnel();

		private:
			TcpSocket m_s1;
			TcpSocket m_s2;
			std::atomic_bool m_running;
			std::thread m_thread1;
			std::thread m_thread2;
			const std::string m_replace_from;
			const std::string m_replace_to;

			void ResendLoop(TcpSocket* sock_in, TcpSocket* sock_out);
		};

		TcpListener m_listener;
		std::list<Tunnel> m_tunnels;
	};
}