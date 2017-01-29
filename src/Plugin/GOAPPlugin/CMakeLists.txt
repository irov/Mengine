set( MY_LIB_NAME GOAPPlugin )

ADD_FILTER(
src
	GOAPPlugin.h
	GOAPPlugin.cpp       
	
	PybindChain.h
	PybindChain.cpp
	PybindSource.h
	PybindSource.cpp
	PybindTask.h
	PybindTask.cpp
	
	PybindCallbackProvider.h
	PybindCallbackProvider.cpp
	PybindFunctionProvider.h
	PybindFunctionProvider.cpp
	PybindGuardProvider.h
	PybindGuardProvider.cpp
	PybindIfProvider.h
	PybindIfProvider.cpp
	PybindScopeProvider.h
	PybindScopeProvider.cpp
	PybindSwitchProvider.h
	PybindSwitchProvider.cpp
)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
        ${THIRDPARTY_DIR}/GOAP/include
		${THIRDPARTY_DIR}/pybind/include
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME} 
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    Logger
    Core
    Codec
    Archive
    Factory
    Kernel    
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}GOAP_static${MENGINE_LIB_SUFFIX}
	${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}pybind_static${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)