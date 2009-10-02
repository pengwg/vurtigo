
# List of source files.
set(CARTO_SRC_FILES
  CartoReaderPlugin.cpp
  CartoReaderUI.cpp
  CartoFileReader.cpp
)

# MOC files are the ones the use Q_OBJECT
set (CARTO_MOC_FILES
  CartoReaderPlugin.h
  CartoReaderUI.h
)
set (UI_FILES_CARTO
  CartoReader.ui
)

qt4_wrap_ui(UI_CARTO_OUT ${UI_FILES_CARTO})
qt4_wrap_cpp(CARTO_MOC_FILES_OUT ${CARTO_MOC_FILES} )

include_directories( 
  ${CMAKE_BINARY_DIR}/Plugins/CartoReader 
)

add_library(CartoReader SHARED 
  ${CARTO_SRC_FILES} 
  ${UI_CARTO_OUT} 
  ${CARTO_MOC_FILES_OUT}
)

target_link_libraries(CartoReader 
  ${QT_LIBRARIES} 
  VurtigoBase 
)
