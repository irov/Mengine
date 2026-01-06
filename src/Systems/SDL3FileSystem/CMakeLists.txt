MENGINE_PROJECT(SDL3FileSystem)

ADD_FILTER(
src
    SDL3FileSystem.h
    SDL3FileSystem.cpp
    
    SDL3FileGroupDirectory.cpp
    SDL3FileGroupDirectory.h
    SDL3FileGroupDirectoryFactory.h
    SDL3FileGroupDirectoryFactory.cpp
    SDL3FileInputStream.h
    SDL3FileInputStream.cpp
    SDL3MutexFileInputStream.h
    SDL3MutexFileInputStream.cpp
    SDL3FileOutputStream.cpp
    SDL3FileOutputStream.h
    SDL3FileMapped.h
    SDL3FileMapped.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL3/include)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL3)



