SET( MY_LIB_NAME SDLTimerSystem )

ADD_FILTER(
src
    SDLTimerSystem.h
    SDLTimerSystem.cpp
)

add_definitions(-DUNICODE)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER System
)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/SDL2/include
)

SET( APPLICATION_DEPENDENCIES
    ${APPLICATION_DEPENDENCIES}
    ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
    ${APPLICATION_DEPENDENCIES}
    PARENT_SCOPE
)
