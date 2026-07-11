MENGINE_PROJECT(POSIXFileSystem)

ADD_FILTER(
src
    POSIXFileSystem.h
    POSIXFileSystem.cpp

    POSIXFileGroupDirectory.h
    POSIXFileGroupDirectory.cpp
    POSIXFileGroupDirectoryFactory.h
    POSIXFileGroupDirectoryFactory.cpp

    POSIXFileInputStream.h
    POSIXFileInputStream.cpp
    POSIXMutexFileInputStream.h
    POSIXMutexFileInputStream.cpp
    POSIXFileOutputStream.h
    POSIXFileOutputStream.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
