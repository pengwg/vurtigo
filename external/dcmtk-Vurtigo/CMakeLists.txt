
#############
# Find DCMTK
#############
set(TEMP_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET( DCMTK_DIR "" CACHE PATH "Root of DCMTK source tree." )
MARK_AS_ADVANCED( CLEAR DCMTK_DIR )

IF ( NOT DCMTK_DIR )
message(STATUS "Warning: DCMTK_DIR has not been set. Checking default file paths...")
ENDIF ( NOT DCMTK_DIR )

find_package(DCMTK REQUIRED)
set(CMAKE_MODULE_PATH ${TEMP_PATH})

include_directories(
  ${DCMTK_INCLUDE_DIR}
)

###########
# Vurtigo Component
###########

set ( DCMTK_VURTIGO_SRC
  DICOMFileReader.cpp
  DICOMImageData.cpp
)

add_library(DCMTKVurtigo SHARED 
  ${DCMTK_VURTIGO_SRC}
)
target_link_libraries (DCMTKVurtigo
  ${QT_LIBRARIES}
  ${DCMTK_LIBRARIES}
  vtkFiltering
  vtkCommon
  vtkRendering
)
