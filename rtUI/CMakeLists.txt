set (RTUI_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtMainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTextPropertyDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPropertyChooserDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/pointPlacementDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtAxesProperties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtRegistration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/customQVTKWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtOptions2DView.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ControlGraphWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PointTableWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VtkRangeWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/HoverPoints.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VtkPiecewiseGraph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtRenderOptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VtkColorTable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtOrientationMarkerWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtWindowLevelDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cineWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ObjectSelectionComboBox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FilterDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtAxesActor.cpp
  CACHE INTERNAL "Vurtigo rtUI Source Files"
)


set (RTUI_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/rtMainWindow.h
  ${CMAKE_HOME_DIRECTORY}/include/rtTextPropertyDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/rtPropertyChooserDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/pointPlacementDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/rtAxesProperties.h
  ${CMAKE_HOME_DIRECTORY}/include/rtRegistration.h
  ${CMAKE_HOME_DIRECTORY}/include/rtOptions2DView.h
  ${CMAKE_HOME_DIRECTORY}/include/GraphWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/ControlGraphWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/PointTableWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/VtkRangeWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/HoverPoints.h
  ${CMAKE_HOME_DIRECTORY}/include/VtkPiecewiseGraph.h
  ${CMAKE_HOME_DIRECTORY}/include/rtRenderOptions.h
  ${CMAKE_HOME_DIRECTORY}/include/VtkColorTable.h
  ${CMAKE_HOME_DIRECTORY}/include/rtOrientationMarkerWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/customQVTKWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/rtWindowLevelDialog.h
  ${CMAKE_HOME_DIRECTORY}/include/cineWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/ObjectSelectionComboBox.h
  ${CMAKE_HOME_DIRECTORY}/include/FilterDialog.h
  CACHE INTERNAL "rtUI Files to be processed by Qt first"
)
