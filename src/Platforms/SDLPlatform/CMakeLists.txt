SET(MY_LIB_NAME SDLPlatform)

ADD_FILTER(
src
    SDLPlatform.h
    SDLPlatform.cpp
    SDLDynamicLibrary.h
    SDLDynamicLibrary.cpp
    SDLDateTimeProvider.h
    SDLDateTimeProvider.cpp
    SDLInput.h
    SDLInput.cpp   
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(${MY_LIB_NAME} ${SRC_FILES})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Shlwapi.lib)
ENDIF()