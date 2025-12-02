#pragma once
#include <string>
#include <map>
#include "connection/TcpSocket.h"
#include "utils/MakeException.h"

MAKE_EXCEPTION(protocol_textbased, std::exception);

namespace protocol
{
	class TextBased
	{
	public:
		TextBased(connection::TcpSocket&& socket, const std::string& protocol_name);
		~TextBased();

	protected:
		struct Answer
		{
			std::string code;
			std::map<std::string, std::string> headers;
			std::string body;
		};

		Answer request(const std::string& method, const std::string& url, const std::map<std::string, std::string>&headers);

	private:
		const std::string m_protocol_name;
		connection::TcpSocket m_socket;

		Answer parse_header(const char* data, size_t size);
	};
}