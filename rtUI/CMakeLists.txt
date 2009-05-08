
set (MOC_FILES
  rtMainWindow.h
  rtTextPropertyDialog.h
  rtPropertyChooserDialog.h
  rtAxesProperties.h
  rtOptions2DView.h
)

set (SRC_FILES
  rtMainWindow.cpp
  rtTextPropertyDialog.cpp
  rtPropertyChooserDialog.cpp
  rtAxesProperties.cpp
  rtImagePlaneWidget.cpp
  customQVTKWidget.cpp
  rtOptions2DView.cpp
)


qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

add_library(rtUI ${SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(rtUI ${QT_LIBRARIES} vtkFiltering vtkCommon rtControl rtPlugin)
