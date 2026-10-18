MENGINE_PROJECT(AAudioSoundSystem)

ADD_FILTER(
src
    AAudioSoundSystem.cpp
    AAudioSoundSystem.h
    AAudioSoundMixer.cpp
    AAudioSoundMixer.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel aaudio)
