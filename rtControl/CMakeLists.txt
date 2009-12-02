set ( RTCONTROL_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtObjectManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTimeManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtMessage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCameraControl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtConfigOptions.cpp
  CACHE INTERNAL "Vurtigo rtControl Source Files"
)

set (RTCONTROL_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/rtTimeManager.h
  ${CMAKE_HOME_DIRECTORY}/include/rtObjectManager.h
  ${CMAKE_HOME_DIRECTORY}/include/rtCameraControl.h
  CACHE INTERNAL "rtControl Files to be processed by Qt first"
)

