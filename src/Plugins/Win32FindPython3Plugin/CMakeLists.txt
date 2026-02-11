MENGINE_PROJECT(Win32FindPython3Plugin)

ADD_FILTER(
src
    Win32FindPython3Interface.h

    Win32FindPython3Service.cpp
    Win32FindPython3Service.h

    Win32FindPython3Plugin.cpp
    Win32FindPython3Plugin.h
    Win32FindPython3Plugin.def
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_WIN32_FINDPYTHON3)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)