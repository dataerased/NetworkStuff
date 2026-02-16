#pragma once
#include "TextBased.h"

namespace protocol
{
	class Http : public TextBased
	{
	public:
		Http(connection::TcpSocket&& socket, const std::string& host);
		~Http();

		TextBased::Answer get(const std::string& url = "/");

	private:
		const std::string m_host;

	};
}