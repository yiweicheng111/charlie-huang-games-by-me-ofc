cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 20)

add_library(Shared STATIC "packet.h" "packet.cpp")
target_include_directories(
	Shared PUBLIC
	"${PROJECT_SOURCE_DIR}/Library/vendor/glm"
	"${PROJECT_SOURCE_DIR}/Library/vendor/cereal"
	"${PROJECT_SOURCE_DIR}/Library/vendor/meshoptimizer"
	"${PROJECT_SOURCE_DIR}/Library/vendor/entt"
)
