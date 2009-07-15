
# List of source files.
set(SRC_FILES
  ScreenCapturePlugin.cpp
  ScreenCaptureUI.cpp
  ScreenCaptureThread.cpp
  JpgWriter.cpp
)

# MOC files are the ones the use Q_OBJECT
set (MOC_FILES
  ScreenCapturePlugin.h
  ScreenCaptureUI.h
  ScreenCaptureThread.h
  JpgWriter.h
)
set (UI_FILES_SCREEN
  ScreenCapture.ui
)

qt4_wrap_ui(UI_SCREEN_OUT ${UI_FILES_SCREEN})
qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES} )

include_directories( ${CMAKE_BINARY_DIR}/Plugins/ScreenCapture )

add_library(ScreenCapture SHARED ${SRC_FILES} ${UI_SCREEN_OUT} ${MOC_FILES_OUT})
target_link_libraries(ScreenCapture ${QT_LIBRARIES} rtObject rtPlugin)
