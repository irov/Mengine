MENGINE_PROJECT(AndroidSoundSystem)

ADD_FILTER(
src
    AndroidSoundSystem.h
    AndroidSoundSystem.cpp
    AndroidSoundBuffer.h
    AndroidSoundBuffer.cpp
    AndroidSoundSource.h
    AndroidSoundSource.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenSLES)
