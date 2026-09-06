MENGINE_PROJECT(XlsExportPlugin)

ADD_FILTER(
src
    XlsExportPlugin.h
    XlsExportPlugin.cpp
    XlsExportPlugin.def

    README.md
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_XLSEXPORT)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_TARGET_WINDOWS)
    if(NOT MENGINE_PLUGIN_WIN32_FINDPYTHON3)
        message(FATAL_ERROR "XlsExportPlugin requires MENGINE_PLUGIN_WIN32_FINDPYTHON3")
    endif()

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Win32FindPython3Plugin)
elseif(MENGINE_TARGET_MACOS)
    if(NOT MENGINE_PLUGIN_MACOS_FINDPYTHON3)
        message(FATAL_ERROR "XlsExportPlugin requires MENGINE_PLUGIN_MACOS_FINDPYTHON3")
    endif()

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} MacOSFindPython3Plugin)
else()
    message(FATAL_ERROR "XlsExportPlugin is only supported on Windows and macOS")
endif()
