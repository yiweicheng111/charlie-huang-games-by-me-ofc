cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE EngineFiles 
    "Scripting/*.cpp" 
    "Scripting/*.h"
    "Editor/*.cpp" 
    "Editor/*.h"
    "Components/*.cpp" 
    "Components/*.h"
    "CharlieCore/*.cpp" 
    "CharlieCore/*.h"
    "CharlieCore/audio/*.cpp" 
    "CharlieCore/audio/*.h"
    "${PROJECT_SOURCE_DIR}/Library/vendor/lua/*.c"
)

add_library(CharlieEngine STATIC ${EngineFiles})
target_include_directories(CharlieEngine PUBLIC
    "${PROJECT_SOURCE_DIR}/Library/include"
    "${PROJECT_SOURCE_DIR}/Library/vendor"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxCore"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxBase"
    "${PROJECT_SOURCE_DIR}/Renderer/GfxRenderer"
    "${PROJECT_SOURCE_DIR}/CharlieEngine"
)


target_link_libraries(CharlieEngine PUBLIC
    "${PROJECT_SOURCE_DIR}/Library/lib/glfw3.lib"
    opengl32.lib
)
