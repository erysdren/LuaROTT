
include(GNUInstallDirs)

# install exe
install(TARGETS rotten DESTINATION ${CMAKE_INSTALL_BINDIR})

# install supplementary files on unix
if(UNIX)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_16x16.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps RENAME rott.png)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_32x32.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps RENAME rott.png)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_64x64.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps RENAME rott.png)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_128x128.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps RENAME rott.png)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/rott_256x256.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps RENAME rott.png)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/rott.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps RENAME rott.svg)
	install(FILES ${PROJECT_SOURCE_DIR}/misc/rotten.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()
