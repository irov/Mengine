MENGINE_PROJECT(AndroidSoundSystem)

ADD_FILTER(
src
    AndroidSoundSystem.cpp
    AndroidSoundSystem.h
    AndroidSoundSystemExtensionInterface.h

    AndroidSoundBufferBase.cpp
    AndroidSoundBufferBase.h
    AndroidSoundBufferMemory.cpp
    AndroidSoundBufferMemory.h
    AndroidSoundBufferStream.cpp
    AndroidSoundBufferStream.h
    AndroidSoundSource.cpp
    AndroidSoundSource.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenSLES)
