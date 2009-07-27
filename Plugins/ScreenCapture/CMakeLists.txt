
# List of source files.
set(SCREEN_CAP_SRC_FILES
  ScreenCapturePlugin.cpp
  ScreenCaptureUI.cpp
  ScreenCaptureThread.cpp
  JpgWriter.cpp
  BmpWriter.cpp
  AviWriter.cpp
)

# MOC files are the ones the use Q_OBJECT
set (SCREEN_CAP_MOC_FILES
  ScreenCapturePlugin.h
  ScreenCaptureUI.h
  ScreenCaptureThread.h
  JpgWriter.h
  BmpWriter.h
  AviWriter.h
)
set (UI_FILES_SCREEN
  ScreenCapture.ui
)

qt4_wrap_ui(UI_SCREEN_OUT ${UI_FILES_SCREEN})
qt4_wrap_cpp(SCREEN_CAP_MOC_FILES_OUT ${SCREEN_CAP_MOC_FILES} )

include_directories( ${CMAKE_BINARY_DIR}/Plugins/ScreenCapture )

add_library(ScreenCapture SHARED ${SCREEN_CAP_SRC_FILES} ${UI_SCREEN_OUT} ${SCREEN_CAP_MOC_FILES_OUT})
target_link_libraries(ScreenCapture ${QT_LIBRARIES} VurtigoBase)
