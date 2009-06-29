
# List of source files.
set(SRC_FILES
  TestSuitePlugin.cpp
  TestSuiteUI.cpp
  TestSuiteBasic.cpp
)

# MOC files are the ones the use Q_OBJECT
set (MOC_FILES
  TestSuitePlugin.h
  TestSuiteUI.h
  TestSuiteBasic.h
)
set (UI_FILES_TESTSUITE
  TestSuite.ui
)

qt4_wrap_ui(UI_TESTSUITE_OUT ${UI_FILES_TESTSUITE})
qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES} )

include_directories( ${CMAKE_BINARY_DIR}/Plugins/TestSuite )

add_library(TestSuite SHARED ${SRC_FILES} ${UI_TESTSUITE_OUT} ${MOC_FILES_OUT} )
target_link_libraries(TestSuite ${QT_LIBRARIES} rtObject rtPlugin)
