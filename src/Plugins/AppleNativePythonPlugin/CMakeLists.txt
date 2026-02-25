MENGINE_PROJECT(AppleNativePythonPlugin)

ADD_FILTER(
src
    AppleNativePythonInterface.h

    AppleNativePythonCallbackDesc.h
    AppleNativePythonCallbackDesc.mm

    AppleNativePythonPlugin.h
    AppleNativePythonPlugin.mm
    
    AppleNativePythonScriptEmbedding.h
    AppleNativePythonScriptEmbedding.mm
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_NATIVE_PYTHON)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleNativePythonPlugin")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)
