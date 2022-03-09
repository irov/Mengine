MENGINE_PROJECT(Win32FileGroupPlugin)

ADD_FILTER(
src
    Win32FileGroupPlugin.h
    Win32FileGroupPlugin.cpp
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
    Win32FileHelper.h
    Win32FileHelper.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_WIN32_FILEGROUP)