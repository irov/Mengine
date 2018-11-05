SET( MY_LIB_NAME StringizeService )

ADD_FILTER(
src
	StringizeService.h
	StringizeService.cpp
    ConstStringHolderMemory.h
    ConstStringHolderMemory.cpp
    #ConstStringHolderStringExternal.h
	#ConstStringHolderStringExternal.cpp
    #ConstStringHolderStringSTL.h
    #ConstStringHolderStringSTL.cpp
    #ConstStringHolderStringSize.h 
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Services
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
