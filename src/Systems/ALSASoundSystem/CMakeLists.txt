MENGINE_PROJECT(ALSASoundSystem)

ADD_FILTER(
src
    ALSASoundSystem.cpp
    ALSASoundSystem.h
)

ADD_MENGINE_LIBRARY(Systems)

find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${ALSA_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Kernel ${ALSA_LIBRARIES} Threads::Threads)
