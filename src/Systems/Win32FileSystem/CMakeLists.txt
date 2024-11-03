MENGINE_PROJECT(Win32FileSystem)

ADD_FILTER(
src
    Win32FileSystem.h
    Win32FileSystem.cpp
    
    Win32FileGroupDirectory.cpp
    Win32FileGroupDirectory.h
    Win32FileGroupDirectoryFactory.cpp
    Win32FileGroupDirectoryFactory.h
    Win32FileInputStream.h
    Win32FileInputStream.cpp
    Win32MutexFileInputStream.h
    Win32MutexFileInputStream.cpp
    Win32FileOutputStream.cpp
    Win32FileOutputStream.h
    Win32FileMapped.h
    Win32FileMapped.cpp
    Win32FileMappedInputStream.h
    Win32FileMappedInputStream.cpp
    Win32ConcatenateFileHelper.h
    Win32ConcatenateFileHelper.cpp
)

ADD_MENGINE_LIBRARY(Systems)

ADD_DEPENDENCIES(${PROJECT_NAME} Windows)