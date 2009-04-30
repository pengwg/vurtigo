
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
)

set (MOC_FILES
  rtDataObject.h
  rtRenderObject.h
  rtLabelDataObject.h
  rt3dPointBufferDataObject.h
  rtNoneDataObject.h
  rtCathDataObject.h
  rt3dVolumeDataObject.h
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

add_library(rtObject SHARED ${SRC_FILES} ${OBJ_SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(rtObject ${QT_LIBRARIES} vtkCommon vtkRendering vtkVolumeRendering)
