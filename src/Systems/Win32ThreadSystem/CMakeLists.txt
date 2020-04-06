MENGINE_PROJECT(Win32ThreadSystem)

ADD_FILTER(
src
    Win32ThreadSystem.cpp
    Win32ThreadSystem.h
    Win32ThreadIdentity.cpp
    Win32ThreadIdentity.h
    Win32ThreadMutex.h
    Win32ThreadMutex.cpp
)

ADD_MENGINE_LIBRARY(Systems)