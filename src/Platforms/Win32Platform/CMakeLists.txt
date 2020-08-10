MENGINE_PROJECT(Win32Platform)

ADD_FILTER(
src
    Win32Platform.h
    Win32Platform.cpp
    Win32DynamicLibrary.h
    Win32DynamicLibrary.cpp
    Win32MouseEvent.h
    Win32MouseEvent.cpp
    Win32DateTimeProvider.h
    Win32DateTimeProvider.cpp
    Win32AlreadyRunningMonitor.h
    Win32AlreadyRunningMonitor.cpp
    Win32AntifreezeMonitor.h
    Win32AntifreezeMonitor.cpp
    Win32CriticalErrorsMonitor.h
    Win32CriticalErrorsMonitor.cpp
)

ADD_MENGINE_LIBRARY()

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi.lib)
ENDIF()