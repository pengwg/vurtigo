# List of source files.
set(CATH_HISTORY_SRC_FILES
  CathHistoryPlugin.cpp
  CathHistoryUI.cpp
  HistoryData.cpp
)

# MOC files are the ones the use Q_OBJECT
set (CATH_HISTORY_MOC_FILES
  CathHistoryPlugin.h
  CathHistoryUI.h
  HistoryData.h
)
set (UI_FILES_CATH_HISTORY
  CathHistory.ui
)

qt4_wrap_ui(UI_CATH_HISTORY_OUT ${UI_FILES_CATH_HISTORY})
qt4_wrap_cpp(CATH_HISTORY_MOC_FILES_OUT ${CATH_HISTORY_MOC_FILES} )

include_directories( 
  ${CMAKE_BINARY_DIR}/Plugins/CathHistory
)

add_library(CathHistory SHARED
  ${CATH_HISTORY_SRC_FILES}
  ${UI_CATH_HISTORY_OUT}
  ${CATH_HISTORY_MOC_FILES_OUT}
)

target_link_libraries(CathHistory
  ${QT_LIBRARIES} 
  VurtigoBase
)
