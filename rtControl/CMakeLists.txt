set ( RTCONTROL_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtObjectManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTimeManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCameraControl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtConfigOptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtHelpManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtApplication.cpp
  CACHE INTERNAL "Vurtigo rtControl Source Files"
)

set (RTCONTROL_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/rtTimeManager.h
  ${CMAKE_HOME_DIRECTORY}/include/rtObjectManager.h
  ${CMAKE_HOME_DIRECTORY}/include/rtCameraControl.h
  ${CMAKE_HOME_DIRECTORY}/include/rtHelpManager.h
  ${CMAKE_HOME_DIRECTORY}/include/rtApplication.h
  CACHE INTERNAL "rtControl Files to be processed by Qt first"
)

