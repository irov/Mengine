SET( MY_LIB_NAME ToolUtils )

ADD_FILTER(
src
    ToolLogger.h
    ToolLogger.cpp
	ToolUtils.h
	ToolUtils.cpp
)

ADD_LIBRARY(${MY_LIB_NAME} ${SRC_FILES})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Shlwapi)
