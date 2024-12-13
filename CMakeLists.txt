cmake_minimum_required(VERSION 3.28)
project(netrott
	DESCRIPTION "SDL3 multiplayer-focused port of Rise of the Triad"
	HOMEPAGE_URL "https://github.com/erysdren/NetROTT/"
	LANGUAGES C
	VERSION 1.1.3
)

option(NETROTT_SHAREWARE "Build shareware version" OFF)
option(NETROTT_WERROR "Enable -Werror on build" OFF)
set(NETROTT_DATADIR "" CACHE STRING "Game data file directory")
set(NETROTT_SUFFIX "" CACHE STRING "Executable file name suffix")

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_mixer REQUIRED CONFIG REQUIRED COMPONENTS SDL3_mixer-shared)
find_package(SDL3_net REQUIRED CONFIG REQUIRED COMPONENTS SDL3_net-shared)

file(GLOB NETROTT_SOURCES
	${PROJECT_SOURCE_DIR}/rott/byteordr.c
	${PROJECT_SOURCE_DIR}/rott/cin_actr.c
	${PROJECT_SOURCE_DIR}/rott/cin_efct.c
	${PROJECT_SOURCE_DIR}/rott/cin_evnt.c
	${PROJECT_SOURCE_DIR}/rott/cin_glob.c
	${PROJECT_SOURCE_DIR}/rott/cin_main.c
	${PROJECT_SOURCE_DIR}/rott/cin_util.c
	${PROJECT_SOURCE_DIR}/rott/dosutil.c
	${PROJECT_SOURCE_DIR}/rott/dukemusc.c
	${PROJECT_SOURCE_DIR}/rott/engine.c
	${PROJECT_SOURCE_DIR}/rott/fx_mixer.c
	${PROJECT_SOURCE_DIR}/rott/ipxsetup.c
	${PROJECT_SOURCE_DIR}/rott/isr.c
	${PROJECT_SOURCE_DIR}/rott/m_misc2.c
	${PROJECT_SOURCE_DIR}/rott/modexlib.c
	${PROJECT_SOURCE_DIR}/rott/rt_actor.c
	${PROJECT_SOURCE_DIR}/rott/rt_battl.c
	${PROJECT_SOURCE_DIR}/rott/rt_build.c
	${PROJECT_SOURCE_DIR}/rott/rt_cfg.c
	${PROJECT_SOURCE_DIR}/rott/rt_com.c
	${PROJECT_SOURCE_DIR}/rott/rt_crc.c
	${PROJECT_SOURCE_DIR}/rott/rt_datadir.c
	${PROJECT_SOURCE_DIR}/rott/rt_debug.c
	${PROJECT_SOURCE_DIR}/rott/rt_dmand.c
	${PROJECT_SOURCE_DIR}/rott/rt_door.c
	${PROJECT_SOURCE_DIR}/rott/rt_draw.c
	${PROJECT_SOURCE_DIR}/rott/rt_err.c
	${PROJECT_SOURCE_DIR}/rott/rt_fixed.c
	${PROJECT_SOURCE_DIR}/rott/rt_floor.c
	${PROJECT_SOURCE_DIR}/rott/rt_game.c
	${PROJECT_SOURCE_DIR}/rott/rt_in.c
	${PROJECT_SOURCE_DIR}/rott/rt_main.c
	${PROJECT_SOURCE_DIR}/rott/rt_map.c
	${PROJECT_SOURCE_DIR}/rott/rt_menu.c
	${PROJECT_SOURCE_DIR}/rott/rt_msg.c
	${PROJECT_SOURCE_DIR}/rott/rt_net.c
	${PROJECT_SOURCE_DIR}/rott/rt_playr.c
	${PROJECT_SOURCE_DIR}/rott/rt_rand.c
	${PROJECT_SOURCE_DIR}/rott/rt_scale.c
	${PROJECT_SOURCE_DIR}/rott/rt_sound.c
	${PROJECT_SOURCE_DIR}/rott/rt_stat.c
	${PROJECT_SOURCE_DIR}/rott/rt_state.c
	${PROJECT_SOURCE_DIR}/rott/rt_str.c
	${PROJECT_SOURCE_DIR}/rott/rt_ted.c
	${PROJECT_SOURCE_DIR}/rott/rt_util.c
	${PROJECT_SOURCE_DIR}/rott/rt_vid.c
	${PROJECT_SOURCE_DIR}/rott/rt_view.c
	${PROJECT_SOURCE_DIR}/rott/scriplib.c
	${PROJECT_SOURCE_DIR}/rott/vgatext.c
	${PROJECT_SOURCE_DIR}/rott/w_wad.c
	${PROJECT_SOURCE_DIR}/rott/winrott.c
	${PROJECT_SOURCE_DIR}/rott/z_zone.c
)

if(WIN32)
	set(NETROTT_SOURCES ${NETROTT_SOURCES} ${PROJECT_SOURCE_DIR}/misc/netrott.rc)
endif()

string(LENGTH "${NETROTT_SUFFIX}" NETROTT_SUFFIX_LEN)
if(NETROTT_SUFFIX_LEN GREATER 0)
	string(CONCAT NETROTT_EXEC "${CMAKE_PROJECT_NAME}" "-" "${NETROTT_SUFFIX}")
else()
	set(NETROTT_EXEC ${CMAKE_PROJECT_NAME})
endif()

if(${NETROTT_SHAREWARE})
	set(NETROTT_TITLE "NetROTT (Shareware)")
else()
	set(NETROTT_TITLE "NetROTT")
endif()

add_executable(${NETROTT_EXEC} ${NETROTT_SOURCES})
target_link_libraries(${NETROTT_EXEC} PRIVATE SDL3::SDL3 SDL3_mixer::SDL3_mixer SDL3_net::SDL3_net)
target_compile_definitions(${NETROTT_EXEC} PRIVATE PACKAGE_STRING="${NETROTT_TITLE} ${CMAKE_PROJECT_VERSION}")
target_compile_definitions(${NETROTT_EXEC} PRIVATE PACKAGE_TARNAME="${CMAKE_PROJECT_NAME}")

if(MSVC)
	set_property(TARGET ${NETROTT_EXEC} PROPERTY C_STANDARD 17)
	# silence warnings about POSIX functions
	target_compile_options(${NETROTT_EXEC} PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -Dstrcasecmp=_stricmp -Dalloca=_alloca)
	# silence unsigned/signed and integer truncation warnings
	target_compile_options(${NETROTT_EXEC} PRIVATE /wd4267 /wd4244)
endif()

if(${NETROTT_SHAREWARE})
	target_compile_definitions(${NETROTT_EXEC} PRIVATE SHAREWARE=1)
endif()

if(${NETROTT_WERROR} AND NOT MSVC)
	target_compile_options(${NETROTT_EXEC} PRIVATE -Werror)
endif()

if(${NETROTT_DATADIR})
	target_compile_definitions(${NETROTT_EXEC} PRIVATE DATADIR="${NETROTT_DATADIR}")
endif()

include(GNUInstallDirs)
if(UNIX)
	configure_file(${PROJECT_SOURCE_DIR}/misc/netrott.desktop.in ${PROJECT_BINARY_DIR}/${NETROTT_EXEC}.desktop @ONLY)
	configure_file(${PROJECT_SOURCE_DIR}/doc/netrott.6.in ${PROJECT_BINARY_DIR}/${NETROTT_EXEC}.6 @ONLY)
	install(FILES ${PROJECT_BINARY_DIR}/${NETROTT_EXEC}.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/netrott.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps RENAME ${NETROTT_EXEC}.svg)
	install(FILES ${PROJECT_BINARY_DIR}/${NETROTT_EXEC}.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
endif()
install(TARGETS ${NETROTT_EXEC} DESTINATION ${CMAKE_INSTALL_BINDIR})
