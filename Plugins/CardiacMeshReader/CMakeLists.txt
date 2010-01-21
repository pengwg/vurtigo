
# List of source files.
set(MESH_SRC_FILES
  CardiacMeshReaderPlugin.cpp
  CardiacMeshReaderUI.cpp
)

# MOC files are the ones that use Q_OBJECT
set (MESH_MOC_FILES
  CardiacMeshReaderPlugin.h
  CardiacMeshReaderUI.h
)
set (UI_FILES_MESH
  CardiacMeshReader.ui
)

qt4_wrap_ui(UI_MESH_OUT ${UI_FILES_MESH})
qt4_wrap_cpp(MESH_MOC_FILES_OUT ${MESH_MOC_FILES} )

include_directories( 
  ${CMAKE_BINARY_DIR}/Plugins/CardiacMeshReader
)

add_library(CardiacMeshReader SHARED
  ${MESH_SRC_FILES}
  ${UI_MESH_OUT}
  ${MESH_MOC_FILES_OUT}
)

target_link_libraries(CardiacMeshReader
  ${QT_LIBRARIES} 
  VurtigoBase 
  DCMTKVurtigo
)
