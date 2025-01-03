MENGINE_PROJECT(AppleNativePythonPlugin)

ADD_FILTER(
src
    AppleNativePythonPlugin.h
    AppleNativePythonPlugin.mm
    
    AppleNativePythonInterface.h
    
    AppleNativePythonService.h
    AppleNativePythonService.mm
    
    AppleNativePythonScriptEmbedding.h
    AppleNativePythonScriptEmbedding.mm
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_NATIVE_PYTHON)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)
