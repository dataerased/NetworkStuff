#include "TextBased.h"

namespace protocol
{
	TextBased::TextBased(connection::TcpSocket&& socket, const std::string& protocol_name)
		: m_socket(std::move(socket))
		, m_protocol_name(protocol_name)
	{
	}

	TextBased::~TextBased()
	{
	}
	
	TextBased::Answer TextBased::request(const std::string& method, const std::string& url, const std::map<std::string, std::string>& headers)
	{
		std::string req = method + " " + url + " " + m_protocol_name + "\r\n";
		for (const auto& [k, v] : headers)
		{
			req += k + ": " + v + "\r\n";
		}
		req += "\r\n";
		auto written = m_socket.write(req.data(), static_cast<int>(req.size()));
		if (written != req.size())
			throw exception::protocol_textbased("failed to write request");

		TextBased::Answer answer{};

		std::string buffer{};
		buffer.resize(2049);

		int current_pos = 0;
		constexpr int data_portion = 512;

		current_pos += m_socket.read(&buffer[current_pos], 1);
		while (true)
		{
			if (current_pos + data_portion > buffer.size())
			{
				buffer.resize((std::min)(buffer.size() * 2, (size_t)1024 * 1024 * 10));
			}

			auto readed = m_socket.read(&buffer[current_pos], data_portion, std::chrono::milliseconds(10));
			auto pos = buffer.rfind("\r\n\r\n", current_pos + readed);

			if (pos != std::string::npos)
			{
				pos += 4; // include \r\n\r\n

				answer = parse_header(buffer.data(), pos);
				if (answer.headers.contains("Content-Length"))
				{
					auto body_size = std::stoul(answer.headers.at("Content-Length"));
					answer.body.resize(body_size);

					auto already_readed_body = current_pos + readed - pos;
					memcpy(&answer.body[0], buffer.data() + pos, already_readed_body);
					m_socket.read(&answer.body[already_readed_body], static_cast<int>(body_size - already_readed_body));
				}
				// TO-DO: add read until close?
				break;
			}

			current_pos += readed;
		}

		return answer;
	}

	TextBased::Answer TextBased::parse_header(const char* data, size_t size)
	{
		Answer result{};

		std::vector<std::string> lines = [&, data]() mutable {
			std::vector<std::string> result;
			int last_index = 0;
			for (int i = 0; i < size; ++i)
			{
				if (data[i] == '\r'
					&& data[i + 1] == '\n')
				{
					result.push_back(std::string(data + last_index, i - last_index));
					last_index = i + 2;
					++i;
				}
			}
			return result;
		}();

		auto it = lines.begin();
		result.code = *it;

		for (++it; it != lines.end() - 1; ++it)
		{
			auto sep = it->find(':');
			if (sep == std::string::npos)
			{
				result.headers[*it] = "";
			}
			else
			{
				result.headers[it->substr(0, sep)] = it->substr(sep + 2);
			}
		}

		return result;
	}
}