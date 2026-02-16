# CMakeList.txt : CMake project for NetworkThings, include source and define
# project specific logic here.
#

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ..
    .
)

# Add source to this project's executable.
add_library (NetworkStuffLib STATIC
	"utils/NoCopy.h" 
	"utils/NoMove.h" 
	"utils/Logger.h" 
	"utils/Logger.cpp" 
	"utils/Utils.h" 
	"utils/Utils.cpp"
	"utils/SocketUtils.h" 
	"utils/SocketUtils.cpp"
    "utils/ProxyChainHelper.h" 
	"utils/ProxyChainHelper.cpp" 
	"utils/SafeHolder.h"
	 
	 
	"connection/Ip.cpp" 
	"connection/Ip.h" 
	"connection/Endpoint.h" 
	"connection/Endpoint.cpp" 
	"connection/TcpSocket.cpp" 
	"connection/TcpSocket.h" 
	"connection/SocketFactory.h" 
	"connection/SocketFactory.cpp" 
	"connection/TcpListener.cpp" 
	"connection/TcpListener.h" 
	"connection/Socks5.cpp" 
	"connection/Socks5.h" 
	"connection/LocalTcpTunnel.h" 
	"connection/LocalTcpTunnel.cpp"
	"protocol/Rtsp.h"
	"protocol/Rtsp.cpp"
	"protocol/TextBased.h" 
	"protocol/TextBased.cpp" 
	"protocol/Http.h" 
	"protocol/Http.cpp" 
	"utils/MakeException.h")

if(WIN32)
    target_link_libraries(NetworkStuffLib PRIVATE ws2_32)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NetworkStuffLib PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
