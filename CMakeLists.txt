cmake_minimum_required(VERSION 3.24)

# set the project name
project(launcher CXX)

# get source files
file(GLOB_RECURSE CLIENT_FILES "src/love/client/*.cpp")
file(GLOB_RECURSE SERVER_FILES "src/love/server/*.cpp")
file(GLOB_RECURSE COMMON_FILES "src/love/common/*.cpp")
file(GLOB_RECURSE EXAMPLE_GAME_CLIENT_FILES "src/example_game/client/*.cpp")
file(GLOB_RECURSE EXAMPLE_GAME_SERVER_FILES "src/example_game/server/*.cpp")
file(GLOB RESOURCE_FILE "products/assets/resources.rc")

# add the executable
add_library(love_common SHARED "${COMMON_FILES}")
add_library(love_server SHARED "${SERVER_FILES}")
add_library(love_client SHARED "${CLIENT_FILES}")
add_executable(game "src/example_game/example_game.cpp;${EXAMPLE_GAME_CLIENT_FILES}")
add_executable(host "src/example_game/example_host.cpp;${EXAMPLE_GAME_SERVER_FILES}")
add_executable(launcher "src/example_game/example_launcher.cpp")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set compiler flags
set(CMAKE_CPP_FLAGS
	-pipe
	-Wall -Werror -Wno-volatile
	-m64 -Ofast -Os
	-Og -g -fno-eliminate-unused-debug-symbols
	-std=c++23
)
set(CMAKE_L_FLAGS
    -s
)

if(WIN32)

set(CMAKE_L_FLAGS
    ${CMAKE_L_FLAGS}
	-mwindows
)

else()

# TODO Check for XLIB or XCB and define VK_USE_PLATFORM_XLIB_KHR or VK_USE_PLATFORM_XCB_KHR

endif()

target_compile_options(love_client PRIVATE ${CMAKE_CPP_FLAGS} -DBUILDING_LOVE_CLIENT)
target_link_options(love_client PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(love_server PRIVATE ${CMAKE_CPP_FLAGS} -DBUILDING_LOVE_SERVER)
target_link_options(love_server PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(love_common PRIVATE ${CMAKE_CPP_FLAGS} -DBUILDING_LOVE_COMMON)
target_link_options(love_common PRIVATE ${CMAKE_L_FLAGS})

target_compile_options(game PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(game PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(host PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(host PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(launcher PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(launcher PRIVATE ${CMAKE_L_FLAGS})

# set include paths
include_directories("lib/include/" "src/")
link_directories("lib/")
target_include_directories(love_client PRIVATE "lib/include/" "src/")
target_link_directories(love_client PRIVATE "lib/" "build/")
target_include_directories(love_server PRIVATE "lib/include/" "src/")
target_link_directories(love_server PRIVATE "lib/" "build/")
target_include_directories(love_common PRIVATE "lib/include/" "src/")
target_link_directories(love_common PRIVATE "lib/" "build/")

target_include_directories(game PRIVATE "lib/include/" "src/")
target_link_directories(game PRIVATE "lib/" "build/")
target_include_directories(host PRIVATE "lib/include/" "src/")
target_link_directories(host PRIVATE "lib/" "build/")
target_include_directories(launcher PRIVATE "lib/include/" "src/")
target_link_directories(launcher PRIVATE "lib/" "build/")

# link libraries
if (WIN32)
# use static libraries for Windows because Microsoft is lame...
set(COMMON_LIBS
	-Wl,--out-implib,liblove_common.dll.a
	-Wl,-Bstatic -lpthread -llzma
	-Wl,-Bdynamic -lgcc -lstdc++ -ldbghelp
)
set(SERVER_LIBS
	-Wl,--out-implib,liblove_server.dll.a
	-Wl,-Bdynamic -llove_common
)
set(CLIENT_LIBS
	-Wl,--out-implib,liblove_client.dll.a
	-Wl,-Bstatic -lglfw3 -lsndfile -lOpenAL32.dll
	-Wl,-Bdynamic -lgcc -lstdc++ -llove_common -llove_server
)
else()
set(COMMON_LIBS
	-Wl,-Bdynamic -lgcc -lstdc++ -lpthread -llzma-5
)
set(SERVER_LIBS
	-Wl,-Bdynamic -lgcc -lstdc++ -llove_common
)
set(CLIENT_LIBS
	-Wl,-Bdynamic -lgcc -lstdc++ -lglfw3 -lOpenAL32 -lsndfile -llove_common -llove_server
)
endif()
set(GAME_LIBS
	-Wl,-Bdynamic -llove_common -llove_client -llove_server
)
set(HOST_LIBS
	-Wl,-Bdynamic -llove_common -llove_server
)

target_link_libraries(love_common PRIVATE ${COMMON_LIBS})
target_link_libraries(love_server PRIVATE ${SERVER_LIBS})
target_link_libraries(love_client PRIVATE ${CLIENT_LIBS})
target_link_libraries(game PRIVATE ${GAME_LIBS})
target_link_libraries(host PRIVATE ${HOST_LIBS})
target_link_libraries(launcher PRIVATE ${COMMON_LIBS})

# set resources
target_sources(launcher PRIVATE ${RESOURCE_FILE})
