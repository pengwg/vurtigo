
set ( RTCUSTOMWIDGETS_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtBox2DOutline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTexture2DPlane.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCrosshair2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPolyData2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtSingle3DPointPipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ObjectControlWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Plane2DControlWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Points3DControlWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCompassWidgetPipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtArrowPipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTubePipeline.cpp
  CACHE INTERNAL "Custom widgets that implement functionality above the standard VTK"
)

set (RTCUSTOMWIDGETS_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/ObjectControlWidget.h
  CACHE INTERNAL "Custom widgets moc files that implement functionality above the standard VTK"
)

