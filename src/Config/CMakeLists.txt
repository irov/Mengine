SET( MY_LIB_NAME Config )

ADD_FILTER(
src
    ArrayString.h
    Blobject.h
    Char.h
    String.h
    StringRegex.h
    VectorString.h
    Stringstream.h
    Typedef.h
    Floats.h
    Vector.h
    Deque.h
    Map.h
    UnorderedMap.h
    List.h
    Tuple.h
    Regex.h
    Atomic.h
    Lambda.h
    Define.h
	Config.h
    Export.h
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
