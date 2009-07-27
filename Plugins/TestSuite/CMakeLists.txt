
# List of source files.
set(TEST_SUITE_SRC_FILES
  TestSuitePlugin.cpp
  TestSuiteUI.cpp
  TestSuiteBasic.cpp
)

# MOC files are the ones the use Q_OBJECT
set (TEST_SUITE_MOC_FILES
  TestSuitePlugin.h
  TestSuiteUI.h
  TestSuiteBasic.h
)
set (UI_FILES_TESTSUITE
  TestSuite.ui
)

qt4_wrap_ui(UI_TESTSUITE_OUT ${UI_FILES_TESTSUITE})
qt4_wrap_cpp(TEST_SUITE_MOC_FILES_OUT ${TEST_SUITE_MOC_FILES} )

include_directories( ${CMAKE_BINARY_DIR}/Plugins/TestSuite )

add_library(TestSuite SHARED ${TEST_SUITE_SRC_FILES} ${UI_TESTSUITE_OUT} ${TEST_SUITE_MOC_FILES_OUT} )
target_link_libraries(TestSuite ${QT_LIBRARIES} VurtigoBase)
