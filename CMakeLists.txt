
# project
cmake_minimum_required(VERSION 3.5)
project(ROTTEN LANGUAGES C VERSION 1.1.2)

# requirements
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Lua 5.1 REQUIRED)

# sources
file(GLOB ROTTEN_SOURCES ${PROJECT_SOURCE_DIR}/source/*.c)

# executable
add_executable(rotten ${ROTTEN_SOURCES})
target_link_libraries(rotten PUBLIC SDL2::SDL2)
target_link_libraries(rotten PUBLIC SDL2_mixer)
target_link_libraries(rotten PUBLIC ${LUA_LIBRARY})
target_include_directories(rotten PUBLIC ${LUA_INCLUDE_DIR})
target_compile_definitions(rotten PRIVATE PACKAGE_STRING="${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
target_compile_definitions(rotten PRIVATE PACKAGE_TARNAME="${CMAKE_PROJECT_NAME}")
