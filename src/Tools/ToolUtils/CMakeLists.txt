MENGINE_PROJECT(ToolUtils)

ADD_FILTER(
src
    ToolLogger.h
    ToolLogger.cpp
	ToolUtils.h
	ToolUtils.cpp
)

ADD_LIBRARY(${PROJECT_NAME} ${SRC_FILES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi)
