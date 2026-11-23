MENGINE_PROJECT(Win32NativePythonPlugin)

ADD_FILTER(
src
    Win32NativePythonPlugin.h
    Win32NativePythonPlugin.cpp
    Win32NativePythonPlugin.def
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        Win32NativePythonSemaphoreListener.h
        Win32NativePythonSemaphoreListener.cpp

        Win32NativePythonScriptEmbedding.h
        Win32NativePythonScriptEmbedding.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_WIN32_NATIVE_PYTHON)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} WindowsEnvironment)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PythonEnvironment)
endif()
