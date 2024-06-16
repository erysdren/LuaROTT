
#
# cmake settings
#

cmake_minimum_required(VERSION 3.5)
project(ROTTEN
	DESCRIPTION "An enhanced port of Rise of the Triad based on SDL3"
	HOMEPAGE_URL "https://github.com/erysdren/ROTTEN/"
	LANGUAGES C
	VERSION 1.1.3
)

#
# options
#

option(ROTTEN_PEDANTIC "Enable all pedantic warnings" OFF)
option(ROTTEN_UBSAN "Enable undefined behaviour sanitizer" OFF)
option(ROTTEN_ASAN "Enable address sanitizer" OFF)
option(ROTTEN_LSAN "Enable leak sanitizer" OFF)
option(ROTTEN_ANALYZER "Enable code analyzer" OFF)
option(ROTTEN_INSTALL "Enable install target" ON)
option(ROTTEN_DEB "Enable .deb file target" OFF)

#
# requirements
#

include(FindPkgConfig)
pkg_search_module(SDL3 REQUIRED sdl3)
pkg_search_module(SDL3_MIXER REQUIRED sdl3-mixer)
pkg_search_module(SDL3_NET REQUIRED sdl3-net)

#
# sources
#

file(GLOB ROTTEN_SOURCES ${PROJECT_SOURCE_DIR}/source/*.c ${PROJECT_SOURCE_DIR}/source/thirdp/*.c)

if(WIN32)
	set(ROTTEN_SOURCES ${ROTTEN_SOURCES} ${PROJECT_SOURCE_DIR}/misc/rotten.rc)
endif()

#
# executable
#

# rotten target function
function(add_rotten_exec exec_name exec_defs)
	add_executable(${exec_name} ${ROTTEN_SOURCES})
	target_compile_definitions(${exec_name} PUBLIC ${exec_defs})
	target_link_libraries(${exec_name} PUBLIC ${SDL3_LIBRARIES} ${SDL3_MIXER_LIBRARIES} ${SDL3_NET_LIBRARIES})
	target_include_directories(${exec_name} PUBLIC ${SDL3_INCLUDE_DIRS} ${SDL3_MIXER_INCLUDE_DIRS} ${SDL3_NET_INCLUDE_DIRS})
	target_compile_definitions(${exec_name} PUBLIC PACKAGE_STRING="${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
	target_compile_definitions(${exec_name} PUBLIC PACKAGE_TARNAME="${CMAKE_PROJECT_NAME}")

	if(MSVC)
		set_property(TARGET ${exec_name} PROPERTY C_STANDARD 17)
		target_compile_options(${exec_name} PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -Dstrcasecmp=_stricmp -Dalloca=_alloca)
		target_compile_options(${exec_name} PRIVATE /wd4267 /wd4244)
	endif()

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(${exec_name} PUBLIC -O3)
	elseif(!MSVC)
		target_compile_options(${exec_name} PUBLIC -g3)
	endif()

	if(ROTTEN_PEDANTIC)
		target_compile_options(${exec_name} PUBLIC -std=gnu99)
		target_compile_options(${exec_name} PUBLIC -Wall -Wextra -pedantic)
		target_compile_options(${exec_name} PUBLIC -Wunused-macros -Wunused-local-typedefs)
		target_compile_options(${exec_name} PUBLIC -Wno-implicit-fallthrough -Wno-sign-compare)
	endif()

	if(ROTTEN_UBSAN)
		target_compile_options(${exec_name} PUBLIC -fsanitize=undefined)
		target_link_options(${exec_name} PUBLIC -fsanitize=undefined)
	endif()

	if(ROTTEN_ASAN)
		target_compile_options(${exec_name} PUBLIC -fsanitize=address)
		target_link_options(${exec_name} PUBLIC -fsanitize=address)
	endif()

	if(ROTTEN_LSAN)
		target_compile_options(${exec_name} PUBLIC -fsanitize=leak)
		target_link_options(${exec_name} PUBLIC -fsanitize=leak)
	endif()

	if(ROTTEN_ANALYZER)
		target_compile_options(${exec_name} PUBLIC -fanalyzer)
		target_link_options(${exec_name} PUBLIC -fanalyzer)
	endif()
endfunction()

# add targets
add_rotten_exec("rotten" "")
add_rotten_exec("rotten_sw" "SHAREWARE=1")

# copy rotten.wad
configure_file(${PROJECT_SOURCE_DIR}/misc/wad/rotten.wad ${PROJECT_BINARY_DIR}/rotten.wad COPYONLY)

#
# install target
#

if(ROTTEN_INSTALL)

	# used for direct installation
	include(GNUInstallDirs)

	# install executables
	install(TARGETS rotten DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(TARGETS rotten_sw DESTINATION ${CMAKE_INSTALL_BINDIR})

	# unix gets supplementary files
	if(UNIX)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten_16x16.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps RENAME rotten.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten_32x32.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps RENAME rotten.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten_64x64.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps RENAME rotten.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten_128x128.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps RENAME rotten.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten_256x256.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps RENAME rotten.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps RENAME rotten.svg)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten_sw.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/wad/rotten.wad DESTINATION ${CMAKE_INSTALL_BINDIR})
	endif()

	# use cpack for debian target
	if(ROTTEN_DEB)
		list(APPEND CPACK_GENERATOR "DEB")
		set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER "erysdren")
	endif()

	# include cpack
	include(CPack)

endif()
