set(MY_LIB_NAME Win32FileGroupPlugin)

ADD_FILTER(
src
	Win32FileGroupPlugin.h
	Win32FileGroupPlugin.cpp
    Win32FileGroupDirectory.cpp
    Win32FileGroupDirectory.h        
	Win32FileInputStream.h
	Win32FileInputStream.cpp
	Win32FileOutputStream.cpp
	Win32FileOutputStream.h
    Win32FileMappedStream.h
    Win32FileMappedStream.cpp
    Win32FileHelper.h
    Win32FileHelper.cpp
)

ADD_MENGINE_LIBRARY(Plugins)

