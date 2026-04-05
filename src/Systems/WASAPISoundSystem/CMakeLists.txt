MENGINE_PROJECT(WASAPISoundSystem)

ADD_FILTER(
src
    WASAPISoundIncluder.h
    WASAPISoundPCMHelper.cpp
    WASAPISoundPCMHelper.h

    WASAPISoundSystem.cpp
    WASAPISoundSystem.h
    WASAPISoundSystemExtensionInterface.h

    WASAPISoundBufferBase.cpp
    WASAPISoundBufferBase.h
    WASAPISoundBufferMemory.cpp
    WASAPISoundBufferMemory.h
    WASAPISoundBufferStream.cpp
    WASAPISoundBufferStream.h
    WASAPISoundSource.cpp
    WASAPISoundSource.h
)

ADD_MENGINE_LIBRARY(Systems)

IF(MENGINE_TOOLCHAIN_MSVC)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Ole32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Avrt)
ENDIF()

IF(MENGINE_TOOLCHAIN_MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} libole32.a)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Avrt)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} atomic)
ENDIF()