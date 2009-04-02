
set ( SRC_FILES
  rtRenderOptions.cpp
  rtDataObject.cpp
  rtRenderObject.cpp
)

set (OBJ_SRC_FILES
  rtLabelRenderOptions.cpp
  rtLabelDataObject.cpp
  rtLabelRenderObject.cpp
)

add_library(rtObject SHARED ${SRC_FILES} ${OBJ_SRC_FILES} )
target_link_libraries(rtObject ${QT_LIBRARIES} vtkCommon)
