
# List of source files.
set(SRC_FILES
  DICOMReaderPlugin.cpp
  DICOMReaderUI.cpp
)

# MOC files are the ones the use Q_OBJECT
set (MOC_FILES
  DICOMReaderPlugin.h
  DICOMReaderUI.h
)
set (UI_FILES_DICOM
  DICOMReader.ui
)

qt4_wrap_ui(UI_DICOM_OUT ${UI_FILES_DICOM})
qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES} )

include_directories( ${CMAKE_BINARY_DIR}/Plugins/DICOMReader )

add_library(DICOMReader SHARED ${SRC_FILES} ${UI_DICOM_OUT} ${MOC_FILES_OUT})
target_link_libraries(DICOMReader ${QT_LIBRARIES} rtObject rtPlugin)
