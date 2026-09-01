cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE EngineFiles 
    "${CMAKE_CURRENT_SOURCE_DIR}/Scripting/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Scripting/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Ray.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Frustum.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Sphere.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/AABB.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioEngine.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/World.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Physics.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/gameIO.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/AssetHandler.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/AssetHandler.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/ObjectController.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CameraController.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Physics.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/World.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/CharliePlayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CharliePlayer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/IO.cpp"
    "${PROJECT_SOURCE_DIR}/Library/vendor/lua/*.c"
    "${PROJECT_SOURCE_DIR}/Library/vendor/miniaudio/*.c"
)

add_library(CharlieEngine STATIC ${EngineFiles})

target_include_directories(CharlieEngine PUBLIC
    "${PROJECT_SOURCE_DIR}/Library/vendor"
	"${PROJECT_SOURCE_DIR}/Library/vendor/enet/include"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxCore"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxRenderer"
    "${PROJECT_SOURCE_DIR}/CharlieEngine"
	"${PROJECT_SOURCE_DIR}/Renderer"
    "${PROJECT_SOURCE_DIR}"
)

find_package(ReactPhysics3D REQUIRED)

target_link_libraries(CharlieEngine PUBLIC
    "${PROJECT_SOURCE_DIR}/Library/lib/glfw3.lib"
    ReactPhysics3D::ReactPhysics3D
    opengl32.lib
)
