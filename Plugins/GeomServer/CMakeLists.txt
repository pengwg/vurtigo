# List of source files.
set(SRC_FILES
  sender.cpp
  geom_client.cpp
  genericMode.cpp
  readOnlyMode.cpp
  writeOnlyMode.cpp
  GeomServerPlugin.cpp
  Converter.cpp
  GeomServerUI.cpp
  SenderSimp.cpp
)

# MOC files are the ones the use Q_OBJECT
set (MOC_FILES
  GeomServerPlugin.h
  GeomServerUI.h
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES} )
add_library(GeomServer SHARED ${SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(GeomServer ${QT_LIBRARIES} rtObject rtPlugin QVTK)
