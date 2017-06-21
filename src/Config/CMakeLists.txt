SET( MY_LIB_NAME Config )

ADD_FILTER(
src
    Blobject.h
    Char.h
    String.h
    Stringstream.h
    Typedef.h
    Floats.h
	Config.h
	cmake_dummy.cpp
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
