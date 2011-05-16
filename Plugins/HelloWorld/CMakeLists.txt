
# List of source files.
set(HELLO_SRC_FILES
  HelloWorldPlugin.cpp
  HelloWorldPluginUI.cpp
  HelloWorldDataObject.cpp
  HelloWorldRenderObject.cpp
  HelloWorldRenderObjectMaker.cpp
)

# MOC files are the ones the use Q_OBJECT
set (HELLO_MOC_FILES
  HelloWorldPlugin.h
  HelloWorldPluginUI.h
  HelloWorldDataObject.h
)

set (UI_FILES_HELLO
  HelloWorldUI.ui
  HelloWorldPluginUI.ui
)

qt4_wrap_ui(UI_HELLO_OUT ${UI_FILES_HELLO} )
qt4_wrap_cpp(HELLO_MOC_FILES_OUT ${HELLO_MOC_FILES} )

include_directories( 
  ${CMAKE_BINARY_DIR}/Plugins/HelloWorld
)

add_library(HelloWorld SHARED ${HELLO_SRC_FILES} ${UI_HELLO_OUT} ${HELLO_MOC_FILES_OUT})
target_link_libraries(HelloWorld ${QT_LIBRARIES} VurtigoBase)
