set (RTUI_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtMainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTextPropertyDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPropertyChooserDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtAxesProperties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtImagePlaneWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/customQVTKWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtOptions2DView.cpp
  CACHE INTERNAL "Vurtigo rtUI Source Files"
)


set (RTUI_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/rtMainWindow.h
  ${CMAKE_HOME_DIRECTORY}/include/rtTextPropertyDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/rtPropertyChooserDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/rtAxesProperties.h
  ${CMAKE_HOME_DIRECTORY}/include/rtOptions2DView.h
  CACHE INTERNAL "rtUI Files to be processed by Qt first"
)