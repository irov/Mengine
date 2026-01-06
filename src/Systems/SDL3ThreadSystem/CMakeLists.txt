MENGINE_PROJECT(SDL3ThreadSystem)

ADD_FILTER(
src
    SDL3ThreadSystem.cpp
    SDL3ThreadSystem.h
    
    SDL3ThreadIdentity.cpp
    SDL3ThreadIdentity.h
    SDL3ThreadIdentityRunner.cpp
    SDL3ThreadIdentityRunner.h
    SDL3ThreadProcessor.cpp
    SDL3ThreadProcessor.h
    SDL3ThreadMutex.cpp
    SDL3ThreadMutex.h
    SDL3ThreadSharedMutex.cpp
    SDL3ThreadSharedMutex.h
    SDL3ThreadConditionVariable.cpp
    SDL3ThreadConditionVariable.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL3/include)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL3)



