set(MY_LIB_NAME PythonFramework)

ADD_FILTER(
PythonFrameworkPlugin
    PythonFrameworkPlugin.cpp
    PythonFrameworkPlugin.h
)

ADD_FILTER(
PythonFramework
    PythonFramework.cpp
    PythonFramework.h
)

ADD_FILTER(
DocumentTraceback
    DocumentTraceback.h
)

ADD_FILTER(
ScriptHolder
	ScriptHolder.cpp
	ScriptHolder.h
)

ADD_FILTER(
ScriptableAffectorCallback
    ScriptableAffectorCallback.cpp
    ScriptableAffectorCallback.h
)

ADD_FILTER(
PythonValueFollower
    PythonValueFollower.cpp
    PythonValueFollower.h
    PythonValueFollowerLinear.cpp
    PythonValueFollowerLinear.h
    PythonValueFollowerAcceleration.cpp
    PythonValueFollowerAcceleration.h
)

ADD_FILTER(
PythonHotSpotEventReceiver
    PythonHotSpotEventReceiver.cpp
    PythonHotSpotEventReceiver.h
)

ADD_FILTER(
PythonSchedule
    PythonScheduleTiming.cpp
    PythonScheduleTiming.h
    PythonSchedulePipe.cpp
    PythonSchedulePipe.h
    PythonScheduleEvent.cpp
    PythonScheduleEvent.h
    DelaySchedulePipe.cpp
    DelaySchedulePipe.h
)   

ADD_FILTER(
Wrapper
    HelperScriptEmbedding.h
    HelperScriptEmbedding.cpp
    SoundScriptEmbedding.h
	SoundScriptEmbedding.cpp
    NodeScriptEmbedding.h
	NodeScriptEmbedding.cpp
    EngineScriptEmbedding.h
    EngineScriptEmbedding.cpp
    MathScriptEmbedding.h
	MathScriptEmbedding.cpp
    EntityScriptEmbedding.h
	EntityScriptEmbedding.cpp
    ConstsScriptEmbedding.h
	ConstsScriptEmbedding.cpp
    GameScriptEmbedding.h
	GameScriptEmbedding.cpp
)

ADD_FILTER(
Generator
    PythonEntityBehavior.h
    PythonEntityBehavior.cpp    
	PythonEntityPrototypeGenerator.h
	PythonEntityPrototypeGenerator.cpp
)

ADD_FILTER(
EventReceiver
    PythonEntityEventReceiver.h
    PythonEntityEventReceiver.cpp
)

ADD_FILTER(
ScriptProviderService
    PythonScriptProviderService.h
    PythonScriptProviderService.cpp
)

ADD_FILTER(
ScriptService
	PythonScriptService.cpp
	PythonScriptService.h
	PythonScriptLogger.h
	PythonScriptLogger.cpp
    PythonScriptModule.h
    PythonScriptModule.cpp
    PythonScriptModuleFinder.h
    PythonScriptModuleFinder.cpp
    PythonScriptModuleLoader.h
    PythonScriptModuleLoader.cpp
    
    PythonScriptCodeData.h
    PythonScriptCodeData.cpp
    DataflowPY.h
    DataflowPY.cpp
    DataflowPYZ.h
    DataflowPYZ.cpp

    ConstStringHolderPythonString.h
    ConstStringHolderPythonString.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmovie/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_PYTHONFRAMEWORK)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind_python${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}Python${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})