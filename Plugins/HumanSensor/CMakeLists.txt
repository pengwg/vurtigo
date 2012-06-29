
set(HUMANSENSOR_SRC_FILES
    HumanSensorPlugin.cpp
    HumanSensor.cpp
)

set(HUMANSENSOR_MOC_FILES
    HumanSensorPlugin.h
    HumanSensor.h
)

include_directories (
    /usr/include/ni
    /usr/include/nite
)

qt4_wrap_cpp(HUMANSENSOR_MOC_FILES_OUT ${HUMANSENSOR_MOC_FILES})

add_library(HumanSensor SHARED ${HUMANSENSOR_SRC_FILES} ${HUMANSENSOR_MOC_FILES_OUT})
target_link_libraries(HumanSensor ${QT_LIBRARIES} VurtigoBase OpenNI XnVNite_1_5_2)
