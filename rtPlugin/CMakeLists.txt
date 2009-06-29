set ( SRC_FILES
  pluginConfigHandler.cpp
  rtPluginLoader.cpp
  rtBaseHandle.cpp
)

set (MOC_FILES
  rtPluginInterface.h
  rtBaseHandle.h
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

add_library(rtPlugin ${SRC_FILES} ${MOC_FILES_OUT} )
target_link_libraries(rtPlugin ${QT_LIBRARIES} rtUI rtObject)
