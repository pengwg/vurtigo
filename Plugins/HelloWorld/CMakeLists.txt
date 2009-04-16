
# List of source files.
set(SRC_FILES
  HelloWorldPlugin.cpp
)

# MOC files are the ones the use Q_OBJECT
set (MOC_FILES
  HelloWorldPlugin.h
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES} )
add_library(HelloWorld SHARED ${SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(HelloWorld ${QT_LIBRARIES} rtObject rtPlugin)