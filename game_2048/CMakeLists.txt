find_package(raylib CONFIG REQUIRED)

add_executable(game_2048 
	"src/main.cc"
	"src/main.h"
	"src/window.cc"
	"src/window.h"
	"src/game.cc"
	"src/game.h"
	"src/logic.cc"
	"src/logic.h"
)

target_link_libraries(game_2048 PRIVATE raylib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET game_2048 PROPERTY CXX_STANDARD 20)
endif()
