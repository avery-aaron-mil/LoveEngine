cmake_minimum_required(VERSION 3.24)

# set the project name
project(launcher CXX)

# get source files
file(GLOB_RECURSE CLIENT_FILES "src/love/client/*.cpp")
file(GLOB_RECURSE SERVER_FILES "src/love/server/*.cpp")
file(GLOB_RECURSE COMMON_FILES "src/love/common/*.cpp")
file(GLOB RESOURCE_FILE "resources.rc")

# add the executable
add_library(common SHARED "${COMMON_FILES}")
add_library(server SHARED "${SERVER_FILES}")
add_library(client SHARED "${CLIENT_FILES}")
add_executable(game "src/example_game/example_game.cpp")
add_executable(host "src/example_game/example_host.cpp")
add_executable(launcher "src/example_game/example_launcher.cpp")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set compiler flags
if(CMAKE_BUILD_TYPE EQUAL DEBUG)

message(STATUS "Compiling in debug mode")

set(CMAKE_CPP_FLAGS
	-Wall -Werror -Wno-volatile
	-O0 -pipe -m64
	-Og -g -DDEBUG
	-std=c++23
)
set(CMAKE_L_FLAGS
	-Wl,--export-all-symbol
)

else()

message(STATUS "Compiling in release mode")

set(CMAKE_CPP_FLAGS
	-Wall -Werror -Wno-volatile
	-Ofast -pipe -m64
	-Os -fdata-sections -ffunction-sections
	-std=c++23
)
set(CMAKE_L_FLAGS
    -s -Wl,--gc-sections
)

if(WIN32)

set(CMAKE_L_FLAGS
    ${CMAKE_L_FLAGS}
	-mwindows
)	

endif()

endif()

target_compile_options(client PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(client PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(server PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(server PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(common PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(common PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(game PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(game PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(host PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(host PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(launcher PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(launcher PRIVATE ${CMAKE_L_FLAGS})

# set include paths
include_directories("lib/include/" "src/")
link_directories("lib/")
target_link_directories(client PRIVATE "lib/" "build/")
target_include_directories(server PRIVATE "lib/include/" "src/")
target_link_directories(server PRIVATE "lib/" "build/")
target_include_directories(common PRIVATE "lib/include/" "src/")
target_link_directories(common PRIVATE "lib/" "build/")
target_include_directories(game PRIVATE "lib/include/" "src/")
target_link_directories(game PRIVATE "lib/" "build/")
target_include_directories(host PRIVATE "lib/include/" "src/")
target_link_directories(host PRIVATE "lib/" "build/")
target_include_directories(launcher PRIVATE "lib/include/" "src/")
target_link_directories(launcher PRIVATE "lib/" "build/")

# link libraries
set(COMMON_LIBS
	-Wl,-Bdynamic -lgcc -lstdc++ -lpthread -llzma-5
)
set(SERVER_LIBS
	${COMMON_LIBS}
	-Wl,-Bdynamic -lcommon
)
set(CLIENT_LIBS
	${SERVER_LIBS}
	-Wl,-Bstatic -l:vulkan-1.lib
	-Wl,-Bdynamic -lglfw3 -lsoft_oal -lsndfile -lserver
)
set(GAME_LIBS
	${COMMON_LIBS}
	-Wl,-Bdynamic -lcommon -lclient
)
set(HOST_LIBS
	${SERVER_LIBS}
	-Wl,-Bdynamic -lserver
)
target_link_libraries(common PRIVATE ${COMMON_LIBS})
target_link_libraries(server PRIVATE ${SERVER_LIBS})
target_link_libraries(client PRIVATE ${CLIENT_LIBS})
target_link_libraries(game PRIVATE ${GAME_LIBS})
target_link_libraries(host PRIVATE ${HOST_LIBS})
target_link_libraries(launcher PRIVATE ${COMMON_LIBS})

# set resources
target_sources(launcher PRIVATE ${RESOURCE_FILE})
