set( MY_LIB_NAME MengePathFinderPlugin )

ADD_FILTER(
ModulePathFinder
    PluginPathFinder.cpp
    PluginPathFinder.h
	ModulePathFinder.cpp
	ModulePathFinder.h
	PathFinderMap.cpp
	PathFinderMap.h
)

INCLUDE_DIRECTORIES(    
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/poly2tri
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
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}poly2tri_static${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)