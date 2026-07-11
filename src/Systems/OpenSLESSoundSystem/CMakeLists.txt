MENGINE_PROJECT(OpenSLESSoundSystem)

ADD_FILTER(
src
    OpenSLESSoundSystem.cpp
    OpenSLESSoundSystem.h
    OpenSLESSoundSystemExtensionInterface.h

    OpenSLESSoundBufferBase.cpp
    OpenSLESSoundBufferBase.h
    OpenSLESSoundBufferMemory.cpp
    OpenSLESSoundBufferMemory.h
    OpenSLESSoundBufferStream.cpp
    OpenSLESSoundBufferStream.h
    OpenSLESSoundSource.cpp
    OpenSLESSoundSource.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenSLES)
