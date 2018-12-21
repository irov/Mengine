SET( MY_LIB_NAME ScriptService )

ADD_FILTER(
src
	ScriptService.cpp
	ScriptService.h
	ScriptLogger.h
	ScriptLogger.cpp
    ScriptModule.h
    ScriptModule.cpp
    ScriptModuleFinder.h
    ScriptModuleFinder.cpp
    ScriptModuleLoader.h
    ScriptModuleLoader.cpp
    
    ScriptCodeData.h
    ScriptCodeData.cpp
    DataflowPY.h
    DataflowPY.cpp
    DataflowPYC.h
    DataflowPYC.cpp

    ConstStringHolderPythonString.h
    ConstStringHolderPythonString.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Services
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
