set ( SRC_FILES
  rtObjectManager.cpp
  rtTimeManager.cpp
)

set (MOC_FILES
  rtTimeManager.h
  rtObjectManager.h
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

add_library(rtControl ${SRC_FILES} ${MOC_FILES_OUT} )
target_link_libraries(rtControl ${QT_LIBRARIES} rtUI rtObject vtkCommon vtkRendering)
