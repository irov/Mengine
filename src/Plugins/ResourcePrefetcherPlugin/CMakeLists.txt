set( MY_LIB_NAME ResourcePrefetcherPlugin )

ADD_FILTER(
src
    ResourcePrefetcherInterface.h
    ResourcePrefetcherServiceInterface.h

	ResourcePrefetcherPlugin.h
    ResourcePrefetcherPlugin.def
	ResourcePrefetcherPlugin.cpp
    
    ResourcePrefetcherService.h
    ResourcePrefetcherService.cpp
    
    DefaultResourcePrefetcher.h
    DefaultResourcePrefetcher.cpp
    DataflowResourcePrefetcher.h
    DataflowResourcePrefetcher.cpp
    ArchiveResourcePrefetcher.h
    ArchiveResourcePrefetcher.cpp
    ImageDecoderResourcePrefetcher.h
    ImageDecoderResourcePrefetcher.cpp
    SoundDecoderResourcePrefetcher.h
    SoundDecoderResourcePrefetcher.cpp
    
    ResourcePrefetcherScriptEmbedding.h
    ResourcePrefetcherScriptEmbedding.cpp
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Engine
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    Kernel
    Engine
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)