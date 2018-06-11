SET( MY_LIB_NAME ServiceTimeline )

ADD_FILTER(
src
	TimelineService.h
	TimelineService.cpp
)

ADD_MENGINE_LIBRARY()

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ) 
    
set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Service
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
