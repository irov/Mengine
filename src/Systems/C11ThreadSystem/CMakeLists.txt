set(MY_LIB_NAME C11ThreadSystem)

ADD_FILTER(
src
	C11ThreadSystem.cpp
	C11ThreadSystem.h
	C11ThreadIdentity.cpp
    C11ThreadIdentity.h
    C11ThreadMutex.h
    C11ThreadMutex.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Systems
)