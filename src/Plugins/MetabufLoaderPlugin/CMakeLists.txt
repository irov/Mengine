set( MY_LIB_NAME MetabufLoaderPlugin )

ADD_FILTER(
src
	LoaderResourceCursorICO.cpp
	LoaderResourceCursorICO.h
	LoaderResourceCursorSystem.cpp
	LoaderResourceCursorSystem.h
	LoaderResourceFile.cpp
	LoaderResourceFile.h
	LoaderResourceHIT.cpp
	LoaderResourceHIT.h
	LoaderResourceImageData.cpp
	LoaderResourceImageData.h
	LoaderResourceImageDefault.cpp
	LoaderResourceImageDefault.h
	LoaderResourceImageSequence.cpp
	LoaderResourceImageSequence.h
	LoaderResourceImageSolid.cpp
	LoaderResourceImageSolid.h
	LoaderResourceImageSubstract.cpp
	LoaderResourceImageSubstract.h
	LoaderResourceImageSubstractRGBAndAlpha.cpp
	LoaderResourceImageSubstractRGBAndAlpha.h
	LoaderResourceMusic.cpp
	LoaderResourceMusic.h
	LoaderResourceShape.cpp
	LoaderResourceShape.h
	LoaderResourceSound.cpp
	LoaderResourceSound.h
	LoaderResourceWindow.cpp
	LoaderResourceWindow.h
	MetabufLoaderPlugin.cpp
	MetabufLoaderPlugin.h    
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/metabuf/include )

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
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
	${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX}
	${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX}
    Kernel
	Engine
    )

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)


