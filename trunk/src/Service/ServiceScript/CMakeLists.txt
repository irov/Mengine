SET( MY_LIB_NAME ServiceScript )

ADD_FILTER(
src
	ScriptEngine.cpp
	ScriptEngine.h
	ScriptLogger.h
	ScriptLogger.cpp
    ScriptModuleFinder.h
    ScriptModuleFinder.cpp
    ScriptModuleLoaderCode.h
    ScriptModuleLoaderCode.cpp
    ScriptModuleLoaderSource.h
    ScriptModuleLoaderSource.cpp
    
    ConstStringHolderPythonString.h
    ConstStringHolderPythonString.cpp
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/python/include
    ) 
    
if( WIN32 )
	INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/python/PC )
endif( WIN32 )
if(APPLE)
	include_directories( ${MENGINE_REPOSITORY}/dependencies/Python )
endif(APPLE)

add_definitions(-DPy_BUILD_CORE)
    
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)
 
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
