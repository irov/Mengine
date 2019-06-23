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
    DataflowPYZ.h
    DataflowPYZ.cpp

    ConstStringHolderPythonString.h
    ConstStringHolderPythonString.cpp
)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/libmath/include
    ) 

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Services
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
