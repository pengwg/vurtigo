set (RTUI_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtMainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTextPropertyDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPropertyChooserDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtAxesProperties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtImagePlaneWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/customQVTKWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtOptions2DView.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ControlGraphWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PointTableWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ProfileWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VtkRangeWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/HoverPoints.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VtkPiecewiseGraph.cpp
  CACHE INTERNAL "Vurtigo rtUI Source Files"
)


set (RTUI_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/rtMainWindow.h
  ${CMAKE_HOME_DIRECTORY}/include/rtTextPropertyDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/rtPropertyChooserDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/rtAxesProperties.h
  ${CMAKE_HOME_DIRECTORY}/include/rtOptions2DView.h
  ${CMAKE_HOME_DIRECTORY}/include/GraphWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/ControlGraphWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/PointTableWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/ProfileWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/VtkRangeWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/HoverPoints.h
  ${CMAKE_HOME_DIRECTORY}/include/VtkPiecewiseGraph.h
  CACHE INTERNAL "rtUI Files to be processed by Qt first"
)
