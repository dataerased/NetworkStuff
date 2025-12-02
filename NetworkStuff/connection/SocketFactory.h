#pragma once
#include <memory>
#include "utils/NoMove.h"
#include "utils/NoCopy.h"
#include "utils/MakeException.h"
#include "TcpSocket.h"
#include "TcpListener.h"
#include "Endpoint.h"

MAKE_EXCEPTION(factory, std::exception);

namespace connection
{
	class SocketFactory : utils::NoMove, utils::NoCopy
	{
	public:
		static SocketFactory& get();
		~SocketFactory();

		TcpSocket connect(const Endpoint& ep);
		TcpListener listen(const Endpoint& ep);
		TcpListener listen(uint16_t port);

	private:
		SocketFactory();

		static std::unique_ptr<SocketFactory> m_instance;

	};
}
