
set ( SRC_FILES
  rtRenderOptions.cpp
)

add_library(rtObject SHARED ${SRC_FILES} )
target_link_libraries(rtObject ${QT_LIBRARIES})
