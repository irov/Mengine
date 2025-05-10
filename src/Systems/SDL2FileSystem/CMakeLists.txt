MENGINE_PROJECT(SDL2FileSystem)

ADD_FILTER(
src
    SDL2FileSystem.h
    SDL2FileSystem.cpp
    
    SDL2FileGroupDirectory.cpp
    SDL2FileGroupDirectory.h
    SDL2FileGroupDirectoryFactory.h
    SDL2FileGroupDirectoryFactory.cpp
    SDL2FileInputStream.h
    SDL2FileInputStream.cpp
    SDL2MutexFileInputStream.h
    SDL2MutexFileInputStream.cpp
    SDL2FileOutputStream.cpp
    SDL2FileOutputStream.h
    SDL2FileMapped.h
    SDL2FileMapped.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2)