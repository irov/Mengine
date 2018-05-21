set( MY_LIB_NAME PythonScriptWrapper )

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
Game
	Game.cpp
	Game.h
	GameAccountProvider.cpp
	GameAccountProvider.h
	GameSoundVolumeProvider.cpp
	GameSoundVolumeProvider.h	
)

ADD_FILTER(
Wrapper
	PythonAnimatableEventReceiver.h
	PythonScriptWrapper.cpp
	PythonScriptWrapper.h
    HelperScriptWrapper.cpp
	SoundScriptWrapper.cpp
	NodeScriptWrapper.cpp
    EngineScriptWrapper.cpp
	MathScriptWrapper.cpp
	EntityScriptWrapper.cpp
	ConstsScriptWrapper.cpp
)

ADD_FILTER(
Generator
	EntityPrototypeGenerator.h
	EntityPrototypeGenerator.cpp
)

INCLUDE_DIRECTORIES(
	${Mengine_SOURCE_DIR}/Engine
	${Mengine_SOURCE_DIR}/Utils 
	${THIRDPARTY_DIR}/pybind/include
	${THIRDPARTY_DIR}/stdex/include
	${THIRDPARTY_DIR}/libmovie/include
    ${THIRDPARTY_DIR}/libmath/include
	${THIRDPARTY_DIR}/utf8/source
    ${THIRDPARTY_DIR}/tinyini/include
    ) 

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

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
