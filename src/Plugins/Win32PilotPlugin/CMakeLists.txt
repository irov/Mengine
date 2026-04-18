MENGINE_PROJECT(Win32PilotPlugin)

ADD_FILTER(
src
    Win32PilotInterface.h

    Win32PilotPlugin.h
    Win32PilotPlugin.def
    Win32PilotPlugin.cpp

    Win32PilotService.h
    Win32PilotService.cpp

    Win32PilotLogger.h
    Win32PilotLogger.cpp

    Win32PilotLoggerListener.h
    Win32PilotLoggerListener.cpp

    Win32PilotAnalyticsEventProvider.h
    Win32PilotAnalyticsEventProvider.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_WIN32_PILOT)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} winpilot)
