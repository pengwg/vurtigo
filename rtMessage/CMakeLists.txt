
set ( RT_MESSAGE_SRC
  rtMessage.cpp
)

add_library(rtMessage SHARED ${RT_MESSAGE_SRC})
target_link_libraries(rtMessage
  ${QT_LIBRARIES}
)
