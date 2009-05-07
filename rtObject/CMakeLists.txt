
set ( SRC_FILES
  rtDataObject.cpp
  rtRenderObject.cpp
)

set (OBJ_SRC_FILES
  rtLabelDataObject.cpp
  rtLabelRenderObject.cpp
  rt3dPointBufferDataObject.cpp
  rt3dPointBufferRenderObject.cpp
  rtNoneDataObject.cpp
  rtNoneRenderObject.cpp
  rtCathDataObject.cpp
  rtCathRenderObject.cpp
  rt3dVolumeDataObject.cpp
  rt3dVolumeRenderObject.cpp
  rt4dVolumeDataObject.cpp
  rt4dVolumeRenderObject.cpp
  rt2dSliceDataObject.cpp
  rt2dSliceRenderObject.cpp
  rtMatrixDataObject.cpp
  rtMatrixRenderObject.cpp
  rtPolyDataObject.cpp
  rtPolyRenderObject.cpp
  rtPieceFuncDataObject.cpp
  rtPieceFuncRenderObject.cpp
  rtColorFuncDataObject.cpp
  rtColorFuncRenderObject.cpp
  rtImageBufferDataObject.cpp
  rtImageBufferRenderObject.cpp
  rt2dPointDataObject.cpp
  rt2dPointRenderObject.cpp
)

set (MOC_FILES
  rtDataObject.h
  rtRenderObject.h
  rtLabelDataObject.h
  rt3dPointBufferDataObject.h
  rtNoneDataObject.h
  rtCathDataObject.h
  rt3dVolumeDataObject.h
  rt4dVolumeDataObject.h
  rt2dSliceDataObject.h
  rtMatrixDataObject.h
  rtPolyDataObject.h
  rtPieceFuncDataObject.h
  rtColorFuncDataObject.h
  rtImageBufferDataObject.h
  rt2dPointDataObject.h
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

add_library(rtObject SHARED ${SRC_FILES} ${OBJ_SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(rtObject ${QT_LIBRARIES} vtkCommon vtkRendering vtkVolumeRendering)
