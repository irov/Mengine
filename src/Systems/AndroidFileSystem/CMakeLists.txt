MENGINE_PROJECT(AndroidFileSystem)

ADD_FILTER(
src
    AndroidFileSystem.h
    AndroidFileSystem.cpp

    AndroidAssetGroupDirectory.cpp
    AndroidAssetGroupDirectory.h
    AndroidAssetInputStream.h
    AndroidAssetInputStream.cpp
    AndroidMutexAssetInputStream.h
    AndroidMutexAssetInputStream.cpp
    
    AndroidFileGroupDirectory.cpp
    AndroidFileGroupDirectory.h
    AndroidFileInputStream.h
    AndroidFileInputStream.cpp
    AndroidMutexFileInputStream.h
    AndroidMutexFileInputStream.cpp
    AndroidFileOutputStream.cpp
    AndroidFileOutputStream.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)