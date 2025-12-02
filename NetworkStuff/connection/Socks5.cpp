#include "Socks5.h"
#include "TcpSocket.h"
#include "SocketFactory.h"
#include "utils/Utils.h"

#include "utils/Logger.h"
using namespace logger;

namespace connection
{
	namespace
	{
#pragma pack(push, 1)
		constexpr uint8_t version_socks5 = 0x5;
		enum Method : uint8_t
		{
			no_auth = 0x00,
			gssapi = 0x01,
			userpass = 0x02,
		};

		struct Hello
		{
			uint8_t version;
			uint8_t n_methods;
			uint8_t method;
		};

		struct HelloReply
		{
			uint8_t version;
			uint8_t method;
		};

		enum Command : uint8_t
		{
			connect = 0x01,
			bind = 0x02,
			udp_associate = 0x03
		};

		enum AddressType : uint8_t
		{
			ip_v4 = 0x01,
			domain_name = 0x03,
			ip_v6 = 0x04,
		};

		struct RequestIp
		{
			uint8_t version;
			uint8_t command;
			uint8_t reserved = 0;
			uint8_t address_type;
			uint32_t ip;
			uint16_t port;
		};

		enum Reply : uint8_t
		{
			succeeded = 0x00,
			socks_fail = 0x01,
			not_allowed = 0x02,
			network_unreachable = 0x03,
			host_unreachable = 0x04,
			refused = 0x05,
			ttl_expored = 0x06,
			command_not_supported = 0x07,
			address_not_supported = 0x08,
		};

		struct RequestIpReply
		{
			uint8_t version;
			uint8_t reply;
			uint8_t reserver = 0;
			uint8_t address_type;
			uint32_t ip;
			uint16_t port;
		};
#pragma pack(pop)
	}

	Socks5::~Socks5()
	{
	}

	TcpSocket Socks5::connect(const std::vector<Endpoint>& chain, const std::chrono::steady_clock::duration& timeout)
	{
		auto begin = std::chrono::steady_clock::now();
		auto get_elpased_timeout = [&]() -> std::chrono::steady_clock::duration {
			auto now = std::chrono::steady_clock::now();
			if (now - begin > timeout)
				return std::chrono::seconds(0);

			return timeout - (begin - now);
		};

		auto it_chain = chain.begin();
		auto socket = SocketFactory::get().connect(*it_chain);

		for (++it_chain; it_chain != chain.end(); ++it_chain)
		{
			try
			{
				connect(socket, *it_chain, get_elpased_timeout());
			}
			catch (const std::exception& ex)
			{
				if (it_chain == chain.end() - 1)
				{
					throw exception::bad_endpoint("chain construction [" + it_chain->to_string() + "]: " + ex.what());
				}
				else
				{
					throw exception::bad_chain("chain is broken [" + it_chain->to_string() + "]: " + ex.what());
				}
			}
		}

		return socket;
	}

	TcpSocket Socks5::connect(const Endpoint& proxy, const Endpoint& ep, const std::chrono::steady_clock::duration& timeout)
	{
		return connect({ proxy, ep }, timeout);
	}

	void Socks5::connect(TcpSocket& socket, const Endpoint& ep, const std::chrono::steady_clock::duration& timeout)
	{
		auto begin = std::chrono::steady_clock::now();
		auto get_elpased_timeout = [&]() -> std::chrono::steady_clock::duration {
			auto now = std::chrono::steady_clock::now();
			if (now - begin > timeout)
				return std::chrono::seconds(0);

			return timeout - (begin - now);
		};

		send_hello(socket, get_elpased_timeout());
		send_request(socket, ep, get_elpased_timeout());
	}

	void Socks5::send_hello(TcpSocket& socket, const std::chrono::steady_clock::duration& timeout)
	{
		Hello hello{ version_socks5, 1, Method::no_auth };
		socket.write(&hello, sizeof(hello));

		HelloReply reply{};
		auto readed = socket.read(&reply, sizeof(reply), timeout);

		if (readed != sizeof(reply))
		{
			throw exception::socks5("bad auth answer size: " + std::to_string(readed));
		}

		if (reply.version != version_socks5 ||
			reply.method != Method::no_auth)
		{
			throw exception::socks5("unsupported auth method");
		}
	}

	void Socks5::send_request(TcpSocket& socket, const Endpoint& ep, const std::chrono::steady_clock::duration& timeout)
	{
		RequestIp request{ version_socks5, Command::connect, 0, AddressType::ip_v4, ep.ip().to_network(), ep.network_port() };
		auto in = socket.write(&request, sizeof(request));
		RequestIpReply reply{};
		auto readed = socket.read(&reply, sizeof(reply), timeout);

		if (readed != sizeof(reply))
		{
			throw exception::socks5("bad request answer size: " + std::to_string(readed));
		}

		if (reply.version != version_socks5 ||
			reply.reply != Reply::succeeded ||
			reply.reserver != 0 ||
			reply.address_type != AddressType::ip_v4)
		{
			if (reply.reply == Reply::refused)
				throw exception::socks5("proxy refuse connection");

			throw exception::socks5("unsupported request answer: " + utils::to_hex(&reply, sizeof(reply)));
		}
	}

}