#pragma once
#include "connection/TcpSocket.h"
#include "TextBased.h"

namespace protocol
{
	class Rtsp : public TextBased
	{
	public:
		Rtsp(connection::TcpSocket&& socket, const std::string& url = "/");
		~Rtsp();

		TextBased::Answer describe();

	private:
		const std::string m_url;
	};
}
