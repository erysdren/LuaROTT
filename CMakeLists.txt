
# project
cmake_minimum_required(VERSION 3.5)
project(ROTTEN LANGUAGES C VERSION 1.1.2)

# options
option(ROTTEN_LUA "Enable unfinished Lua support" OFF)
option(ROTTEN_PEDANTIC "Enable all pedantic warnings" OFF)
option(ROTTEN_UBSAN "Enable UBSAN" OFF)
option(ROTTEN_ASAN "Enable ASAN" OFF)

# add module path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# requirements
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
if(ROTTEN_LUA)
	find_package(Lua REQUIRED)
endif()

# sources
file(GLOB ROTTEN_SOURCES ${PROJECT_SOURCE_DIR}/source/*.c)

# add rc if we're on windows
if(WIN32)
	set(ROTTEN_SOURCES ${ROTTEN_SOURCES} ${PROJECT_SOURCE_DIR}/misc/rott.rc)
endif()

# executable
add_executable(rotten ${ROTTEN_SOURCES})
target_link_libraries(rotten PUBLIC SDL2::Main SDL2::Mixer)
target_compile_definitions(rotten PUBLIC PACKAGE_STRING="${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
target_compile_definitions(rotten PUBLIC PACKAGE_TARNAME="${CMAKE_PROJECT_NAME}")

# install setup
include(${PROJECT_SOURCE_DIR}/cmake/install.cmake)

# lua support
if(ROTTEN_LUA)
	target_link_libraries(rotten PUBLIC ${LUA_LIBRARY})
	target_include_directories(rotten PUBLIC ${LUA_INCLUDE_DIR})
	target_compile_definitions(rotten PUBLIC ROTTEN_LUA=1)
	add_custom_command(
		TARGET rotten POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/scripts/" "${PROJECT_BINARY_DIR}/scripts/"
	)
endif()

# pedantic warnings
if(ROTTEN_PEDANTIC)
	target_compile_options(rotten PUBLIC -std=gnu99)
	target_compile_options(rotten PUBLIC -Wall -Wextra -pedantic)
	target_compile_options(rotten PUBLIC -Wunused-macros -Wunused-local-typedefs)
	target_compile_options(rotten PUBLIC -Wno-implicit-fallthrough -Wno-sign-compare)
endif()

if(MSVC)
	set_property(TARGET rotten PROPERTY C_STANDARD 17)
	target_compile_options(rotten PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -Dstrcasecmp=_stricmp -Dalloca=_alloca)
	target_compile_options(rotten PRIVATE /wd4267 /wd4244)
endif()

# ubsan
if(ROTTEN_UBSAN)
	target_compile_options(rotten PUBLIC -fsanitize=undefined)
	target_link_options(rotten PUBLIC -fsanitize=undefined)
endif()

# asan
if(ROTTEN_ASAN)
	target_compile_options(rotten PUBLIC -fsanitize=address)
	target_link_options(rotten PUBLIC -fsanitize=address)
endif()

# optimizations or debug symbols
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_options(rotten PUBLIC -O3)
elseif(!MSVC)
	target_compile_options(rotten PUBLIC -g3)
endif()
