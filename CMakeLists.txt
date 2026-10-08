cmake_minimum_required(VERSION 4.0)
project(Charlie)
set(CMAKE_CXX_STANDARD 20)


add_subdirectory("${PROJECT_SOURCE_DIR}/Library/vendor/meshoptimizer")
add_subdirectory("${PROJECT_SOURCE_DIR}/Library/vendor/enet")
add_subdirectory("${PROJECT_SOURCE_DIR}/Library/vendor/assimp")

find_package(ReactPhysics3D CONFIG REQUIRED)

add_subdirectory(Server)
add_subdirectory(Renderer)
add_subdirectory(CharlieEngine)
add_subdirectory(CharlieEditor)


add_executable(Charlie Main.cpp)


target_include_directories(
    Charlie PRIVATE
    "${PROJECT_SOURCE_DIR}/Library/include"
    "${PROJECT_SOURCE_DIR}/Library/vendor"
    "${PROJECT_SOURCE_DIR}/Library/vendor/meshoptimizer/src"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxCore"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxRenderer"
    "${PROJECT_SOURCE_DIR}/CharlieEngine"
    "${PROJECT_SOURCE_DIR}/CharlieEditor"
    "${PROJECT_SOURCE_DIR}/Renderer"
     
)
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/Assets"
    "${CMAKE_CURRENT_BINARY_DIR}"
)  


target_link_libraries(Charlie PUBLIC Renderer CharlieEngine CharlieEditor enet assimp meshoptimizer ReactPhysics3D::ReactPhysics3D)
add_dependencies(Charlie CopyAssets)
