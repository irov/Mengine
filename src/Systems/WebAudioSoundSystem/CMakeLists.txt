MENGINE_PROJECT(WebAudioSoundSystem)

ADD_FILTER(
src
    WebAudioBridge.h
    WebAudioSoundSystem.h
    WebAudioSoundSystem.cpp
    WebAudioSoundBuffer.h
    WebAudioSoundBuffer.cpp
    WebAudioSoundSource.h
    WebAudioSoundSource.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_OPTIONS(${PROJECT_NAME} INTERFACE
    "SHELL:--js-library '${CMAKE_CURRENT_SOURCE_DIR}/WebAudioBridge.js'"
)

SET_PROPERTY(TARGET ${PROJECT_NAME} APPEND PROPERTY INTERFACE_LINK_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/WebAudioBridge.js
)
