#include <thread>
#include <vector>
#include "utils/Logger.h"
#include "utils/ProxyChainHelper.h"
#include "connection/LocalTcpTunnel.h"
#include "connection/SocketFactory.h"

using namespace connection;
using namespace logger;

int main()
{
	try
	{
		utils::ProxyChainHelper chain_helper;
		chain_helper.add_proxy({ "127.0.0.1", 4447 }, true);
		chain_helper.add_proxy({ "98.191.0.37", 4145 });
		chain_helper.add_proxy({ "67.201.35.145", 4145 });

		Log() << "Start proxy validation" << std::endl;
		for (int i = 0; i < 5; ++i)
		{
			auto n_proxies = chain_helper.validate_existed();
			Log() << "Validate iteration " << i << " result: " << n_proxies << std::endl;
		}

		auto random_chain = chain_helper.get_chain(2);
		// connect to something

	}
	catch (const std::exception& ex)
	{
		Log() << "Exception: " << ex.what() << std::endl;
	}
	return 0;
}
