set( MY_LIB_NAME Python )

ADD_FILTER(
src     
    PythonScriptWrapper.h
    PythonEventReceiver.h
	PythonAnimatableEventReceiver.h    
)

ADD_FILTER(
dummy
	dummy.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Environment
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
