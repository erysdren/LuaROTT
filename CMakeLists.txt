
#
# cmake settings
#

cmake_minimum_required(VERSION 3.5)
project(ROTTEN
	DESCRIPTION "An enhanced port of Rise of the Triad based on SDL2"
	HOMEPAGE_URL "https://erysdren.me/rotten.html"
	LANGUAGES C
	VERSION 1.1.3
)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

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

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_net REQUIRED)

#
# sources
#

# all all c files in the source folder
file(GLOB ROTTEN_SOURCES ${PROJECT_SOURCE_DIR}/source/*.c)

# add rc file if we're on windows
if(WIN32)
	set(ROTTEN_SOURCES ${ROTTEN_SOURCES} ${PROJECT_SOURCE_DIR}/misc/rott.rc)
endif()

#
# executable
#

# registered
add_executable(rotten ${ROTTEN_SOURCES})
target_link_libraries(rotten PUBLIC SDL2::Main SDL2::Mixer SDL2::Net)
target_compile_definitions(rotten PUBLIC PACKAGE_STRING="${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
target_compile_definitions(rotten PUBLIC PACKAGE_TARNAME="${CMAKE_PROJECT_NAME}")
if(MSVC)
	set_property(TARGET rotten PROPERTY C_STANDARD 17)
	target_compile_options(rotten PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -Dstrcasecmp=_stricmp -Dalloca=_alloca)
	target_compile_options(rotten PRIVATE /wd4267 /wd4244)
endif()

# shareware
add_executable(rotten_sw  ${ROTTEN_SOURCES})
target_compile_definitions(rotten_sw PUBLIC SHAREWARE=1)
target_link_libraries(rotten_sw PUBLIC SDL2::Main SDL2::Mixer SDL2::Net)
target_compile_definitions(rotten_sw PUBLIC PACKAGE_STRING="${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
target_compile_definitions(rotten_sw PUBLIC PACKAGE_TARNAME="${CMAKE_PROJECT_NAME}")
if(MSVC)
set_property(TARGET rotten_sw PROPERTY C_STANDARD 17)
	target_compile_options(rotten_sw PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -Dstrcasecmp=_stricmp -Dalloca=_alloca)
	target_compile_options(rotten_sw PRIVATE /wd4267 /wd4244)
endif()

# copy rotten.wad
configure_file(${PROJECT_SOURCE_DIR}/misc/wad/rotten.wad ${PROJECT_BINARY_DIR}/rotten.wad COPYONLY)

# pedantic warnings
if(ROTTEN_PEDANTIC)
	target_compile_options(rotten PUBLIC -std=gnu99)
	target_compile_options(rotten PUBLIC -Wall -Wextra -pedantic)
	target_compile_options(rotten PUBLIC -Wunused-macros -Wunused-local-typedefs)
	target_compile_options(rotten PUBLIC -Wno-implicit-fallthrough -Wno-sign-compare)
endif()

# unefined behaviour sanitizer
if(ROTTEN_UBSAN)
	target_compile_options(rotten PUBLIC -fsanitize=undefined)
	target_link_options(rotten PUBLIC -fsanitize=undefined)
endif()

# address sanitizer
if(ROTTEN_ASAN)
	target_compile_options(rotten PUBLIC -fsanitize=address)
	target_link_options(rotten PUBLIC -fsanitize=address)
endif()

# leak sanitizer
if(ROTTEN_LSAN)
	target_compile_options(rotten PUBLIC -fsanitize=leak)
	target_link_options(rotten PUBLIC -fsanitize=leak)
endif()

# code analyzer
if(ROTTEN_ANALYZER)
	target_compile_options(rotten PUBLIC -fanalyzer)
	target_link_options(rotten PUBLIC -fanalyzer)
endif()

# optimizations or debug symbols
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_options(rotten PUBLIC -O3)
elseif(!MSVC)
	target_compile_options(rotten PUBLIC -g3)
endif()

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
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_16x16.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps RENAME rott.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_32x32.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps RENAME rott.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_64x64.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps RENAME rott.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_128x128.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps RENAME rott.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_256x256.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps RENAME rott.png)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rott.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps RENAME rott.svg)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
		install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten_sw.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
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
