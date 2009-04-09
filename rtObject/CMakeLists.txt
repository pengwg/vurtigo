
set ( SRC_FILES
  rtDataObject.cpp
  rtRenderObject.cpp
)

set (OBJ_SRC_FILES
  rtLabelDataObject.cpp
  rtLabelRenderObject.cpp
)

set (MOC_FILES
  rtDataObject.h
  rtRenderObject.h
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

add_library(rtObject ${SRC_FILES} ${OBJ_SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(rtObject ${QT_LIBRARIES} vtkCommon vtkRendering)
