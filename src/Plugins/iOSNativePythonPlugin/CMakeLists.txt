MENGINE_PROJECT(iOSNativePythonPlugin)

ADD_FILTER(
src
    iOSNativePythonInterface.h

    iOSNativePythonCallbackDesc.h
    iOSNativePythonCallbackDesc.mm

    iOSNativePythonPlugin.h
    iOSNativePythonPlugin.mm

    iOSNativePythonScriptEmbedding.h
    iOSNativePythonScriptEmbedding.mm
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_NATIVE_PYTHON)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSNativePythonPlugin")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)
