MENGINE_PROJECT(AndroidNativePythonPlugin)

ADD_FILTER(
src
    AndroidNativePythonPlugin.h
    AndroidNativePythonPlugin.cpp
    
    AndroidNativePythonInterface.h
    
    AndroidNativePythonService.h
    AndroidNativePythonService.cpp
    
    AndroidNativePythonHelper.h
    AndroidNativePythonHelper.cpp
    
    AndroidNativePythonScriptEmbedding.h
    AndroidNativePythonScriptEmbedding.cpp
    
    PythonAndroidSemaphoreListener.h
    PythonAndroidSemaphoreListener.cpp
    
    AndroidNativePythonCallback.h
    AndroidNativePythonCallback.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)