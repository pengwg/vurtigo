
set (UI_FILES 
  rtMainWindow.ui
)

set (MOC_FILES
  rtMainWindow.h
)

set (SRC_FILES
  rtMainWindow.cpp
)

qt4_wrap_ui(UI_FILES_OUT ${UI_FILES})
qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(rtUI SHARED ${SRC_FILES} ${UI_FILES_OUT} ${MOC_FILES_OUT})
target_link_libraries(rtUI ${QT_LIBRARIES})
