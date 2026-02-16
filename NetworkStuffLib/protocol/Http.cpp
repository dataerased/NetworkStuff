#include "Http.h"

namespace protocol
{
	Http::Http(connection::TcpSocket&& socket, const std::string& host)
		: TextBased(std::move(socket), "HTTP/1.1")
		, m_host(host)
	{
	}

	Http::~Http()
	{
	}

	TextBased::Answer Http::get(const std::string& url)
	{
		return request("GET", url, { {"Host", m_host}, {"User-Agent", "curl/8.6.0"}, {"Accept", "*/*"} });
	}

}
