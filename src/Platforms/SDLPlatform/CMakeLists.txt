MENGINE_PROJECT(SDLPlatform)

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

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi.lib)
ENDIF()