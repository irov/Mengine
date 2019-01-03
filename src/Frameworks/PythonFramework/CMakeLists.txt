set(MY_LIB_NAME PythonFramework)

ADD_FILTER(
PythonFramework
    PythonFramework.cpp
    PythonFramework.h
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
	EntityPrototypeGenerator.h
	EntityPrototypeGenerator.cpp
)

INCLUDE_DIRECTORIES(
	${THIRDPARTY_DIR}/pybind/include
	${THIRDPARTY_DIR}/stdex/include
	${THIRDPARTY_DIR}/libmovie/include
    ${THIRDPARTY_DIR}/libmath/include
	${THIRDPARTY_DIR}/utf8/source
    ${THIRDPARTY_DIR}/tinyini/include
    ) 

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Frameworks
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python${MENGINE_LIB_SUFFIX}	
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python${MENGINE_LIB_SUFFIX}
	${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}	
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Engine
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
