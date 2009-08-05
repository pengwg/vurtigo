####################
# Setup for DCMTK
####################

set(TEMP_MODULE_PATH  ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})


set(DCMTK_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "DCMTK Root Directory.")
mark_as_advanced(CLEAR DCMTK_DIR)
find_package(DCMTK REQUIRED)

# Restore the state
set(CMAKE_MODULE_PATH ${TEMP_MODULE_PATH})

# List of source files.
set(DICOM_SRC_FILES
  DICOMReaderPlugin.cpp
  DICOMReaderUI.cpp
  DICOMFileReader.cpp
)

# MOC files are the ones the use Q_OBJECT
set (DICOM_MOC_FILES
  DICOMReaderPlugin.h
  DICOMReaderUI.h
)
set (UI_FILES_DICOM
  DICOMReader.ui
)

qt4_wrap_ui(UI_DICOM_OUT ${UI_FILES_DICOM})
qt4_wrap_cpp(DICOM_MOC_FILES_OUT ${DICOM_MOC_FILES} )

include_directories( 
  ${DCMTK_INCLUDE_DIR} 
  ${CMAKE_BINARY_DIR}/Plugins/DICOMReader 
)

add_library(DICOMReader SHARED 
  ${DICOM_SRC_FILES} 
  ${UI_DICOM_OUT} 
  ${DICOM_MOC_FILES_OUT}
)

target_link_libraries(DICOMReader 
  ${QT_LIBRARIES} 
  VurtigoBase 
  ${DCMTK_LIBRARIES}
)
