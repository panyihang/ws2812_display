
# UI
file(GLOB_RECURSE UI_SRCS
    ui/*.c
    ui/*.cpp
)
set(UI_INCS
    ui/
)
# sensor
file(GLOB_RECURSE SENSOR_SRCS
    sensor/*.c
    sensor/*.cpp
)
set(SENSOR_INCS
    sensor/
)
# comm
file(GLOB_RECURSE COMM_SRCS
    comm/*.c
    comm/*.cpp
)
set(COMM_INCS
    comm/
)
idf_component_register(SRCS "main.c" ${UI_SRCS} ${SENSOR_SRCS} ${COMM_SRCS}
                    INCLUDE_DIRS "." ${UI_INCS} ${SENSOR_INCS} ${COMM_INCS}

)

