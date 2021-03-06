
set ( RTOBJECT_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rtDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtLabelDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtLabelRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt3dPointBufferDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt3dPointBufferRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtNoneDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtNoneRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCathDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCathRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt3dVolumeDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt3dVolumeRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt2dSliceDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt2dSliceRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt2dSliceInputColorWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtMatrixDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtMatrixRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPolyDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPolyRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPieceFuncDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtPieceFuncRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtColorFuncDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtColorFuncRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtImageBufferDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtImageBufferRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt2dPointDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt2dPointRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt2DPlotDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt2DPlotRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtEPDataObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtEPRenderObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtEPInfoObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtEPPropertyPointList.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtBasicPointData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtBasic2DPointData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtBasic3DPointData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rt3DTimePointData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCardiacMeshPointData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtNamedInfoPointData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rtCartoPointData.cpp
  CACHE INTERNAL "Vurtigo rtObject Source Files"
)

set (RTOBJECT_MOC_FILES
  ${CMAKE_HOME_DIRECTORY}/include/rtDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtRenderObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtLabelDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt3dPointBufferDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt3dPointBufferRenderObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtNoneDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtCathDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt3dVolumeDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt3dVolumeRenderObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt2dSliceDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt2dSliceInputColorWidget.h
  ${CMAKE_HOME_DIRECTORY}/include/rtMatrixDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtPolyDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtEPDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtPieceFuncDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtColorFuncDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rtImageBufferDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt2dPointDataObject.h
  ${CMAKE_HOME_DIRECTORY}/include/rt2DPlotDataObject.h
  CACHE INTERNAL "rtObject Files to be processed by Qt first"
)

