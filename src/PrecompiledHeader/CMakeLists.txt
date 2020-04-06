MENGINE_PROJECT(PrecompiledHeader)

ADD_FILTER(
src
	PrecompiledHeader.h
	PrecompiledHeader.cpp
)

CREATE_PRECOMPILED_HEADER()

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRC_FILES})

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER PrecompiledHeader
)