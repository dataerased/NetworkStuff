#include "Rtsp.h"

namespace protocol
{
	Rtsp::Rtsp(connection::TcpSocket&& socket, const std::string& url)
		: TextBased(std::move(socket), "RTSP/1.0")
		, m_url(url)
	{
	}

	Rtsp::~Rtsp()
	{
	}

	TextBased::Answer Rtsp::describe()
	{
		return request("DESCRIBE", m_url, { {"CSeq", "2"} });
	}

}
