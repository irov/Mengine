MENGINE_PROJECT(Win32OpButtonPlugin)

ADD_FILTER(
src
    Win32OpButtonInterface.h

    Win32OpButtonPlugin.h
    Win32OpButtonPlugin.def
    Win32OpButtonPlugin.cpp

    Win32OpButtonService.h
    Win32OpButtonService.cpp

    Win32OpButtonLogger.h
    Win32OpButtonLogger.cpp

    Win32OpButtonLoggerListener.h
    Win32OpButtonLoggerListener.cpp

    Win32OpButtonAnalyticsEventProvider.h
    Win32OpButtonAnalyticsEventProvider.cpp
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        Win32OpButtonScriptEmbedding.h
        Win32OpButtonScriptEmbedding.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_WIN32_OPBUTTON)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/opbutton/include)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} WindowsEnvironment)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}opbutton${MENGINE_LIB_SUFFIX})

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PythonEnvironment)
endif()
