MENGINE_PROJECT(POSIXThreadSystem)

ADD_FILTER(
src
    POSIXThreadSystem.cpp
    POSIXThreadSystem.h
    POSIXThreadIdentity.cpp
    POSIXThreadIdentity.h
    POSIXThreadIdentityRunner.cpp
    POSIXThreadIdentityRunner.h
    POSIXThreadProcessor.cpp
    POSIXThreadProcessor.h
    POSIXThreadMutex.cpp
    POSIXThreadMutex.h
    POSIXThreadSharedMutex.cpp
    POSIXThreadSharedMutex.h
    POSIXThreadConditionVariable.cpp
    POSIXThreadConditionVariable.h
)

ADD_MENGINE_LIBRARY(Systems)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()
