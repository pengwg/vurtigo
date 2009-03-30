
set ( SRC_FILES
  rtRenderOptions.cpp
  rtDataObject.cpp
)

add_library(rtObject SHARED ${SRC_FILES} )
target_link_libraries(rtObject ${QT_LIBRARIES})
