cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SourceFiles
	"Server.cpp"
	"Server.h"
)
add_executable(Server ${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp)

target_include_directories(
	Server PUBLIC
	"${PROJECT_SOURCE_DIR}/Library/vendor/enet/include"
	"${PROJECT_SOURCE_DIR}/Library/vendor/entt"
	"${PROJECT_SOURCE_DIR}/Library/vendor/cereal"
	"${PROJECT_SOURCE_DIR}/Library/vendor/assimp/code/Common"
	"${PROJECT_SOURCE_DIR}/Shared"
)
target_link_libraries(Server PRIVATE enet)
