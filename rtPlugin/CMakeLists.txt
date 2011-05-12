set ( RTPLUGIN_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/pluginConfigHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPluginLoader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtBaseHandle.cpp
  CACHE INTERNAL "Vurtigo rtPlugin Source Files"
)

set ( RTPLUGIN_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/rtBaseHandle.h
  CACHE INTERNAL "rtPlugin Files to processed by Qt first"
)

