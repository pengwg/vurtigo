
# List of source files.
set(HELLO_SRC_FILES
  HelloWorldPlugin.cpp
)

# MOC files are the ones the use Q_OBJECT
set (HELLO_MOC_FILES
  HelloWorldPlugin.h
)

qt4_wrap_cpp(HELLO_MOC_FILES_OUT ${HELLO_MOC_FILES} )
add_library(HelloWorld SHARED ${HELLO_SRC_FILES} ${HELLO_MOC_FILES_OUT})
target_link_libraries(HelloWorld ${QT_LIBRARIES} VurtigoBase)