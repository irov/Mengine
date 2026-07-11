MENGINE_PROJECT(ALSASoundSystem)

ADD_FILTER(
src
    ALSASoundPCMHelper.cpp
    ALSASoundPCMHelper.h

    ALSASoundSystem.cpp
    ALSASoundSystem.h
    ALSASoundSystemExtensionInterface.h

    ALSASoundBufferBase.cpp
    ALSASoundBufferBase.h
    ALSASoundBufferMemory.cpp
    ALSASoundBufferMemory.h
    ALSASoundBufferStream.cpp
    ALSASoundBufferStream.h
    ALSASoundSource.cpp
    ALSASoundSource.h
)

ADD_MENGINE_LIBRARY(Systems)

find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${ALSA_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${ALSA_LIBRARIES} Threads::Threads)
