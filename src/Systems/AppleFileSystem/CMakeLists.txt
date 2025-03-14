MENGINE_PROJECT(AppleFileSystem)

ADD_FILTER(
src
    AppleFileSystem.h
    AppleFileSystem.mm
    
    AppleFileGroupDirectory.mm
    AppleFileGroupDirectory.h
    AppleFileGroupDirectoryFactory.h
    AppleFileGroupDirectoryFactory.mm
    AppleFileInputStream.h
    AppleFileInputStream.mm
    AppleMutexFileInputStream.h
    AppleMutexFileInputStream.mm
    AppleFileOutputStream.mm
    AppleFileOutputStream.h
    AppleFileMapped.h
    AppleFileMapped.mm
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)