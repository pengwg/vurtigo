

###########
# DCMTK - Vurtigo Component
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
