# List of source files.
set(ALIGNMENT_TOOL_SRC_FILES
  AlignmentToolPlugin.cpp
  AlignmentToolUI.cpp
  Aligner.cpp
)

# MOC files are the ones the use Q_OBJECT
set (ALIGNMENT_TOOL_MOC_FILES
  AlignmentToolPlugin.h
  AlignmentToolUI.h
  Aligner.h
)
set (UI_FILES_ALIGNMENT_TOOL
  AlignmentTool.ui
)

qt4_wrap_ui(UI_ALIGNMENT_TOOL_OUT ${UI_FILES_ALIGNMENT_TOOL})
qt4_wrap_cpp(ALIGNMENT_TOOL_MOC_FILES_OUT ${ALIGNMENT_TOOL_MOC_FILES} )

include_directories( 
  ${CMAKE_BINARY_DIR}/Plugins/AlignmentTool
)

add_library(AlignmentTool SHARED
  ${ALIGNMENT_TOOL_SRC_FILES}
  ${UI_ALIGNMENT_TOOL_OUT}
  ${ALIGNMENT_TOOL_MOC_FILES_OUT}
)

target_link_libraries(AlignmentTool
  ${QT_LIBRARIES} 
  VurtigoBase
)
