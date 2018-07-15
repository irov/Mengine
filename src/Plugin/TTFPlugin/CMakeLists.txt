set( MY_LIB_NAME TTFPlugin )

ADD_FILTER(
src
    TTFPlugin.h
	TTFPlugin.cpp
    TTFServiceInterface.h
    TTFFont.h
    TTFFont.cpp
    TTFAtlasService.h
    TTFAtlasService.cpp    
    TTFPrototypeGenerator.h
    TTFPrototypeGenerator.cpp
)

#add_definitions(-DMENGINE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils
        ${THIRDPARTY_DIR}/freetype/include
        ${THIRDPARTY_DIR}/pybind/include
        ${THIRDPARTY_DIR}/utf8/source
        ${THIRDPARTY_DIR}/libfe/include
        ${THIRDPARTY_DIR}/tinyini/include
        ${THIRDPARTY_DIR}/libmath/include
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

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
    Kernel
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}freetype${MENGINE_LIB_SUFFIX}	
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}font-effects${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)