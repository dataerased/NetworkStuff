#pragma once
#include <list>
#include "connection/Socks5.h"
#include "MakeException.h"
#include "SafeHolder.h"
#include "SocketUtils.h"

MAKE_EXCEPTION(proxy_chain, std::exception);

namespace utils
{
	class ProxyChainHelper
	{
	public:
		ProxyChainHelper();
		~ProxyChainHelper();

		void add_proxy(const connection::Endpoint& proxy_ep, bool is_critical = false);
		size_t validate_existed(size_t n_threads = 10);

		struct Chain
		{
			std::vector<connection::Endpoint> chain;
			CheckChainResult check_result;
		};
		Chain get_chain(size_t n_non_crit_proxies, bool check = true);

	private:
		SafeHolder<std::vector<connection::Endpoint>> m_critical_proxies;
		SafeHolder<std::list<connection::Endpoint>> m_proxies;

	};
}