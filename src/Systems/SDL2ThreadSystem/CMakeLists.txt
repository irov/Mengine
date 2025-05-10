MENGINE_PROJECT(SDL2ThreadSystem)

ADD_FILTER(
src
    SDL2ThreadSystem.cpp
    SDL2ThreadSystem.h
    
    SDL2ThreadIdentity.cpp
    SDL2ThreadIdentity.h
    SDL2ThreadIdentityRunner.cpp
    SDL2ThreadIdentityRunner.h
    SDL2ThreadProcessor.cpp
    SDL2ThreadProcessor.h
    SDL2ThreadMutex.cpp
    SDL2ThreadMutex.h
    SDL2ThreadSharedMutex.cpp
    SDL2ThreadSharedMutex.h
    SDL2ThreadConditionVariable.cpp
    SDL2ThreadConditionVariable.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2)