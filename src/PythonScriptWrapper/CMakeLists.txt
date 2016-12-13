set( MY_LIB_NAME PythonScriptWrapper )

ADD_FILTER(
ScriptHolder
	ScriptHolder.cpp
	ScriptHolder.h
)

ADD_FILTER(
Wrapper
	ScriptWrapper.h
	ScriptClassWrapper.h
	PythonScriptWrapper.cpp
	PythonScriptWrapper.h
    HelperScriptWrapper.cpp
	SoundScriptWrapper.cpp
	NodeScriptWrapper.cpp
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
	${Mengine_SOURCE_DIR}/Menge
	${Mengine_SOURCE_DIR}/Utils 
	${THIRDPARTY_DIR}/pybind/include
	${THIRDPARTY_DIR}/stdex/include
	${THIRDPARTY_DIR}/libmovie/include
	${THIRDPARTY_DIR}/utf8/source
    ) 

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}python_static${MENGINE_LIB_SUFFIX}	
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}pybind_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}python_static${MENGINE_LIB_SUFFIX}
	${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX}	
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Menge
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
