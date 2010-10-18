
set ( RTCUSTOMWIDGETS_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtBox2DOutline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtTexture2DPlane.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCrosshair2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPolyData2D.cpp
  CACHE INTERNAL "Custom widgets that implement functionality above the standard VTK"
)

set (RTCUSTOMWIDGETS_MOC_FILES
  CACHE INTERNAL "Custom widgets moc files that implement functionality above the standard VTK"
)

