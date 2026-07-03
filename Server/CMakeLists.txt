cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SourceFiles
	"Server.cpp"
	"Server.h"
	"IO.cpp"
)
add_executable(Server ${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp)

target_include_directories(
	Server PUBLIC
	"${PROJECT_SOURCE_DIR}/Library/vendor/enet/include"
	"${PROJECT_SOURCE_DIR}/Library/vendor"
	"${PROJECT_SOURCE_DIR}/CharlieEngine"
	"${CMAKE_SOURCE_DIR}"
)
target_link_libraries(Server PRIVATE enet CharlieEngine)
