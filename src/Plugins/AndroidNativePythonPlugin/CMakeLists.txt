MENGINE_PROJECT(AndroidNativePythonPlugin)

ADD_FILTER(
src
    AndroidNativePythonPlugin.h
    AndroidNativePythonPlugin.cpp
    
    AndroidNativePythonInterface.h
    AndroidNativePythonEventHandlerInterface.h
    
    AndroidNativePythonService.h
    AndroidNativePythonService.cpp
    
    AndroidNativePythonHelper.h
    AndroidNativePythonHelper.cpp
    
    AndroidNativePythonScriptEmbedding.h
    AndroidNativePythonScriptEmbedding.cpp
    
    PythonAndroidSemaphoreListener.h
    PythonAndroidSemaphoreListener.cpp
    
    AndroidNativePythonFunctor.h
    AndroidNativePythonFunctor.cpp
    AndroidNativePythonFunctorVoid.h
    AndroidNativePythonFunctorVoid.cpp
    AndroidNativePythonFunctorBoolean.h
    AndroidNativePythonFunctorBoolean.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)