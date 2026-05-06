cmake_minimum_required(VERSION 4.0)
project(Charlie)
set(CMAKE_CXX_STANDARD 20)
add_executable(Charlie Main.cpp)
file(GLOB SHADER_FILES "${PROJECT_SOURCE_DIR}/Shaders/*")
target_include_directories(
    Charlie PRIVATE
    "${PROJECT_SOURCE_DIR}/Library/include"
    "${PROJECT_SOURCE_DIR}/Library/vendor"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxCore"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxBase"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxRenderer"
    "${PROJECT_SOURCE_DIR}/CharlieEngine"
    "${PROJECT_SOURCE_DIR}/CharlieEditor"
	"${PROJECT_SOURCE_DIR}/CharlieEngine/CharlieCore"
)
add_custom_target(CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/Shaders"
    "${CMAKE_CURRENT_BINARY_DIR}"
)  
add_subdirectory(Renderer)
add_subdirectory(CharlieEngine)
add_subdirectory(CharlieEditor)

target_link_libraries(Charlie PUBLIC Renderer CharlieEngine CharlieEditor "${PROJECT_SOURCE_DIR}/Library/lib/assimp.lib")
add_dependencies(Charlie CopyShaders)
