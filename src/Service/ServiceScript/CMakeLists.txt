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

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Service
)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/libmath/include
    ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
