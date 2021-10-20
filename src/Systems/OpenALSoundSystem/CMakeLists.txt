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

IF(MENGINE_TARGET_ANDROID)
    IF(UNIX)
        string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_CORRECT)
    ELSE()
        SET(CMAKE_BUILD_TYPE_CORRECT ${CMAKE_BUILD_TYPE})
    ENDIF()
    
    add_library(OpenAL32 SHARED IMPORTED)
    SET(OPENAL_IMPORTED_LOCATION_PATH ${MENGINE_REPOSITORY}/gradle/libraries/OpenAL32/build/intermediates/cmake/${CMAKE_BUILD_TYPE_CORRECT}/obj/${ANDROID_ABI}/libopenal.so)
    set_target_properties(OpenAL32 PROPERTIES IMPORTED_LOCATION ${OPENAL_IMPORTED_LOCATION_PATH})
    MESSAGE("OPENAL_IMPORTED_LOCATION_PATH: ${OPENAL_IMPORTED_LOCATION_PATH}")

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenAL32)
ENDIF()