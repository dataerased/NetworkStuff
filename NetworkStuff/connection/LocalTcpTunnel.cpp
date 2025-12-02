#include "LocalTcpTunnel.h"
#include "SocketFactory.h"

namespace connection
{
	LocalTcpTunnel::Tunnel::Tunnel(TcpSocket&& s1, TcpSocket&& s2, const std::string& replace_from, const std::string& replace_to)
		: m_s1(std::move(s1))
		, m_s2(std::move(s2))
		, m_running(true)
		, m_replace_from(replace_from)
		, m_replace_to(replace_to)
	{
		m_thread1 = std::thread(&LocalTcpTunnel::Tunnel::ResendLoop, this, &m_s1, &m_s2);
		m_thread2 = std::thread(&LocalTcpTunnel::Tunnel::ResendLoop, this, &m_s2, &m_s1);
	}

	LocalTcpTunnel::Tunnel::~Tunnel()
	{
		m_running = false;
		if (m_thread1.joinable())
			m_thread1.join();
		if (m_thread2.joinable())
			m_thread2.join();
	}

	void LocalTcpTunnel::Tunnel::ResendLoop(TcpSocket* sock_in, TcpSocket* sock_out)
	{
		try
		{
			bool need_replace = m_replace_from.size();
			std::vector<char> buffer(48 * 1024, 0);
			while (m_running && *sock_in && *sock_out)
			{
				if (auto readed = sock_in->read(&buffer[0], static_cast<int>(buffer.size()), std::chrono::milliseconds(100)))
				{
					bool replaced = false;
					if (need_replace && readed >= m_replace_from.size())
					{
						auto it_end = std::next(buffer.begin(), readed - m_replace_from.size());
						for (auto it = buffer.begin(); it != it_end; ++it)
						{
							if (memcmp(&(*it), m_replace_from.data(), m_replace_from.size()) == 0)
							{
								auto size = std::distance(buffer.begin(), it);

								std::vector<char> new_buffer(readed + m_replace_to.size() - m_replace_from.size(), 0);
								memcpy(&new_buffer[0], buffer.data(), size);
								memcpy(&new_buffer[size], m_replace_to.data(), m_replace_to.size());
								memcpy(&new_buffer[size + m_replace_to.size()], buffer.data() + size + m_replace_from.size(), readed - (size + m_replace_from.size()));

								sock_out->write(new_buffer.data(), new_buffer.size());
								replaced = true;
								break;
							}
						}
					}

					if (!replaced)
					{
						sock_out->write(buffer.data(), readed);
					}
				}
			}
		}
		catch (const std::exception&)
		{
		}
		m_running = false;
	}

	LocalTcpTunnel::LocalTcpTunnel(const std::function<TcpSocket()> creator, uint16_t local_port, const Endpoint& ep)
		: m_listener(SocketFactory::get().listen(local_port))
	{
		m_listener.async_accept([&, local_port, creator, ep](TcpSocket&& socket) {
			m_tunnels.emplace_back(creator(), std::move(socket), "127.0.0.1:" + std::to_string(local_port), ep.to_string());
		});
	}

	LocalTcpTunnel::~LocalTcpTunnel()
	{
	}
}