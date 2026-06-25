cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SourceFiles
	"GfxBase/*.cpp"
	"GfxBase/*.h"
	"GfxBase/*.c"
	"GfxCore/*.cpp"
	"GfxCore/*.h"
	"${PROJECT_SOURCE_DIR}/Library/vendor"
	"GfxRenderer/*.cpp"
	"GfxRenderer/*.h"
)
add_library(Renderer STATIC ${SourceFiles})
target_include_directories(
	Renderer PUBLIC
	"GfxCore"
	"GfxBase"
	"GfxRenderer"
	"${PROJECT_SOURCE_DIR}/Library/include"
	"${PROJECT_SOURCE_DIR}/Library/vendor"
	"${PROJECT_SOURCE_DIR}/CharlieEngine"
	"${PROJECT_SOURCE_DIR}/Renderer"
)
target_link_libraries(
	Renderer PUBLIC
	"${PROJECT_SOURCE_DIR}/Library/lib/glfw3.lib"
	"opengl32.lib"
)
