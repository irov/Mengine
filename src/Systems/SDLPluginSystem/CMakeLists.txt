SET( MY_LIB_NAME SDLPluginSystem )

ADD_FILTER(
src
    SDLDynamicLibrary.h
    SDLDynamicLibrary.cpp
    SDLPluginSystem.h
    SDLPluginSystem.cpp
)

add_definitions(-DUNICODE)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Systems
)

INCLUDE_DIRECTORIES(
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
