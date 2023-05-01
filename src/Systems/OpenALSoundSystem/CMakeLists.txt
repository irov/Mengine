MENGINE_PROJECT(OpenALSoundSystem)

ADD_FILTER(
src
    OpenALSoundIncluder.h
    OpenALSoundBufferBase.cpp
    OpenALSoundBufferBase.h
    OpenALSoundBufferMemory.cpp
    OpenALSoundBufferMemory.h
    OpenALSoundBufferStream.cpp
    OpenALSoundBufferStream.h
    OpenALSoundSource.cpp
    OpenALSoundSource.h
    OpenALSoundSystem.cpp
    OpenALSoundSystem.h
    OpenALSoundErrorHelper.cpp
    OpenALSoundErrorHelper.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/openal-soft/include)

add_definitions(-DAL_LIBTYPE_STATIC)

ADD_MENGINE_LIBRARY(Systems)

IF(MENGINE_TOOLCHAIN_MSVC OR MENGINE_TOOLCHAIN_MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}OpenAL32${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Winmm)
ENDIF()

IF(MENGINE_TARGET_UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}openal${MENGINE_LIB_SUFFIX})
ENDIF()

if(MENGINE_TOOLCHAIN_MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} atomic)
endif()

IF(MENGINE_TARGET_ANDROID)
    IF(UNIX)
        string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_CORRECT)
    ELSE()
        SET(CMAKE_BUILD_TYPE_CORRECT ${CMAKE_BUILD_TYPE})
    ENDIF()

    ADD_LIBRARY_IMPORTED(OPENAL_IMPORTED "${MENGINE_REPOSITORY}/gradle/libraries/OpenAL32/build/intermediates/library_jni/${CMAKE_BUILD_TYPE_CORRECT}/jni/${ANDROID_ABI}/libopenal.so")
ENDIF()