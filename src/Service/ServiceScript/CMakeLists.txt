SET( MY_LIB_NAME ServiceScript )

ADD_FILTER(
src
	ScriptEngine.cpp
	ScriptEngine.h
	ScriptLogger.h
	ScriptLogger.cpp
    ScriptModule.h
    ScriptModule.cpp
    ScriptModuleFinder.h
    ScriptModuleFinder.cpp
    ScriptModuleLoader.h
    ScriptModuleLoader.cpp
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
    ) 
    
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
