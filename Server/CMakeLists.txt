cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SourceFiles
	"Server.cpp"
	"Server.h"
	"IO.cpp"
)
add_executable(Server ${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp)

add_custom_target(CopyAssets2 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/Assets"
    "${CMAKE_CURRENT_BINARY_DIR}"
)  






target_include_directories(
	Server PUBLIC
	"${PROJECT_SOURCE_DIR}/Library/vendor/enet/include"
	"${PROJECT_SOURCE_DIR}/Library/vendor"
	"${PROJECT_SOURCE_DIR}/CharlieEngine"
	"${CMAKE_SOURCE_DIR}"
)
target_link_libraries(Server PRIVATE enet assimp CharlieEngine ReactPhysics3D::ReactPhysics3D)
add_dependencies(Server CopyAssets2)
