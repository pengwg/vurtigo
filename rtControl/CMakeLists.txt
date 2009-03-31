set ( SRC_FILES
  rtObjectManager.cpp
)

add_library(rtControl SHARED ${SRC_FILES} )
target_link_libraries(rtControl ${QT_LIBRARIES} rtUI)
