#include "ProxyChainHelper.h"
#include "SocketUtils.h"
#include <fstream>
#include <random>

namespace utils
{
	ProxyChainHelper::ProxyChainHelper()
	{
	}

	ProxyChainHelper::~ProxyChainHelper()
	{
	}

	size_t ProxyChainHelper::validate_existed(size_t n_threads)
	{
		auto critical_chain = m_critical_proxies.Lock().ref();
		try
		{
			check_chain_speed(critical_chain);
		}
		catch (const std::exception& ex)
		{
			throw exception::proxy_chain(std::string("failed to validate critical chain: ") + ex.what());
		}

		decltype(m_proxies) current(std::move(m_proxies));
		auto check = [&, cc = critical_chain]() {
			while (true)
			{
				connection::Endpoint ep{};
				{
					auto lock = current.Lock();
					if (lock->empty())
						break;

					ep = lock->back();
					lock->pop_back();
				}

				auto chain = cc;
				chain.push_back(ep);

				try
				{
					check_chain_speed(chain);
					m_proxies.Lock()->push_back(ep);
				}
				catch (const std::exception&)
				{
				}
			}
		};

		std::vector<std::thread> threads;
		for (int i = 0; i < n_threads; ++i)
			threads.push_back(std::thread(check));

		for (auto& t : threads)
			if (t.joinable())
				t.join();

		return m_proxies.Lock()->size();

		{
			auto proxies = m_proxies.Lock();
			for (auto it = proxies->begin(); it != proxies->end();)
			{
				auto chain = critical_chain;
				chain.push_back(*it);

				try
				{
					check_chain_speed(chain);
					++it;
				}
				catch (const std::exception&)
				{
					it = proxies->erase(it);
				}
			}
		}
	}

	ProxyChainHelper::Chain ProxyChainHelper::get_chain(size_t n_non_crit_proxies, bool check)
	{
		auto existed_proxies = m_proxies.Lock()->size();

		if (n_non_crit_proxies > existed_proxies)
		{
			throw exception::proxy_chain("not enought not critical proxies: " + std::to_string(existed_proxies) + "/" + std::to_string(n_non_crit_proxies));
		}

		Chain result{};
		result.chain = m_critical_proxies.Lock().ref();
		auto proxies = m_proxies.Lock().ref();

		std::mt19937 engine{ std::random_device{}() };
		std::uniform_int_distribution<size_t> rand{ 0, (std::numeric_limits<size_t>::max)() };

		while (n_non_crit_proxies > result.chain.size() - m_critical_proxies.Lock()->size())
		{
			try
			{
				auto copy = result.chain;
				auto rand_pos = rand(engine) % proxies.size();
				auto it = proxies.begin();
				std::advance(it, rand_pos);
				auto rand_proxy = *it;
				proxies.erase(it);
				copy.push_back(rand_proxy);

				if (check)
					result.check_result = check_chain_speed(copy);
				result.chain = copy;
			}
			catch (const std::exception&)
			{
				if (result.chain.size() + proxies.size() - m_critical_proxies.Lock()->size() < n_non_crit_proxies)
					throw exception::proxy_chain("not enought worked proxies");
			}
		}
		return result;
	}

	void ProxyChainHelper::add_proxy(const connection::Endpoint& proxy_ep, bool is_critical)
	{
		if (is_critical)
		{
			m_critical_proxies.Lock()->push_back(proxy_ep);
		}
		else
		{
			m_proxies.Lock()->push_back(proxy_ep);
		}
	}
}