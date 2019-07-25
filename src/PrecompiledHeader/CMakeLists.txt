set(MY_LIB_NAME PrecompiledHeader)

ADD_FILTER(
src
	PrecompiledHeader.h
	PrecompiledHeader.cpp
)

CREATE_PRECOMPILED_HEADER()

ADD_LIBRARY(${MY_LIB_NAME} STATIC ${SRC_FILES})

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER PrecompiledHeader
)