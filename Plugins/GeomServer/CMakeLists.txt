# List of source files.
set(GEOM_SRC_FILES
  sender.cpp
  genericMode.cpp
  GeomServerPlugin.cpp
  Converter.cpp
  GeomServerUI.cpp
  SenderSimp.cpp
  SenderThread.cpp
)

# MOC files are the ones the use Q_OBJECT
set (GEOM_MOC_FILES
  GeomServerPlugin.h
  GeomServerUI.h
  SenderThread.h
  SenderSimp.h
)

set (UI_FILES_GEOM
  GeomServer.ui
)

qt4_wrap_ui(UI_GEOM_OUT ${UI_FILES_GEOM})

find_file(GEOM_CLIENT_H geom_client.h DOC "The full path to the file geom_client.h")
get_filename_component(GEOM_CLIENT_H_PATH ${GEOM_CLIENT_H} PATH)

find_library(GEOM_CLIENT_LIB GeomClient DOC "The full path to the GeomClient library")

include_directories( ${CMAKE_BINARY_DIR}/Plugins/GeomServer ${GEOM_CLIENT_H_PATH} ${GEOM_CLIENT_H_PATH}/.. ${GEOM_CLIENT_H_PATH}/../GeomData ${GEOM_CLIENT_H_PATH}/../GeomErrorHandler )

qt4_wrap_cpp(GEOM_MOC_FILES_OUT ${GEOM_MOC_FILES} )
add_library(GeomServer SHARED ${GEOM_SRC_FILES} ${UI_GEOM_OUT} ${GEOM_MOC_FILES_OUT})
target_link_libraries(GeomServer ${QT_LIBRARIES} VurtigoBase QVTK ${GEOM_CLIENT_LIB} )
