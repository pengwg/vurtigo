# List of source files.
set(CATH_TRACK_SRC_FILES
  CathTrackingPlugin.cpp
  CathTrackingUI.cpp
  TrackData.cpp
)

# MOC files are the ones the use Q_OBJECT
set (CATH_TRACK_MOC_FILES
  CathTrackingPlugin.h
  CathTrackingUI.h
  TrackData.h
)
set (UI_FILES_CATH_TRACK
  CathTracking.ui
)

qt4_wrap_ui(UI_CATH_TRACK_OUT ${UI_FILES_CATH_TRACK})
qt4_wrap_cpp(CATH_TRACK_MOC_FILES_OUT ${CATH_TRACK_MOC_FILES} )

include_directories( 
  ${CMAKE_BINARY_DIR}/Plugins/CathTracking
)

add_library(CathTracking SHARED
  ${CATH_TRACK_SRC_FILES}
  ${UI_CATH_TRACK_OUT}
  ${CATH_TRACK_MOC_FILES_OUT}
)

target_link_libraries(CathTracking
  ${QT_LIBRARIES} 
  VurtigoBase
)
