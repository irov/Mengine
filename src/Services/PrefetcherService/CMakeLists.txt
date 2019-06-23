SET( MY_LIB_NAME PrefetcherService )

ADD_FILTER(
src
    PrefetcherService.h
	PrefetcherService.cpp    
    ThreadTaskPrefetch.h
    ThreadTaskPrefetch.cpp
    ThreadTaskPrefetchDataflow.h
    ThreadTaskPrefetchDataflow.cpp
    ThreadTaskPrefetchImageDecoder.h
    ThreadTaskPrefetchImageDecoder.cpp
    ThreadTaskPrefetchSoundDecoder.h
    ThreadTaskPrefetchSoundDecoder.cpp
    ThreadTaskPrefetchStream.h
    ThreadTaskPrefetchStream.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Services
)

INCLUDE_DIRECTORIES( 
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
