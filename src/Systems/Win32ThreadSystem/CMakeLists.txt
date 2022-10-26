MENGINE_PROJECT(Win32ThreadSystem)

ADD_FILTER(
src
    Win32ThreadSystem.cpp
    Win32ThreadSystem.h
    Win32ThreadHelper.cpp
    Win32ThreadHelper.h
    Win32ThreadIdentity.cpp
    Win32ThreadIdentity.h
    Win32ThreadIdentityRunner.cpp
    Win32ThreadIdentityRunner.h
    Win32ThreadProcessor.cpp
    Win32ThreadProcessor.h
    Win32ThreadMutex.cpp
    Win32ThreadMutex.h
)

ADD_MENGINE_LIBRARY(Systems)