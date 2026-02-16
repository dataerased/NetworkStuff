# CMakeList.txt : CMake project for NetworkThings, include source and define
# project specific logic here.
#

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ..
    .
)

# Add source to this project's executable.
add_executable (Example
	"main.cpp"
)

target_include_directories(Example PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../NetworkStuffLib
)

if(WIN32)
    target_link_libraries(Example PRIVATE ws2_32 NetworkStuffLib)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Example PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
