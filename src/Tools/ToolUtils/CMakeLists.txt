MENGINE_PROJECT(ToolUtils)

ADD_FILTER(
src
    ToolLogger.h
    ToolLogger.cpp
    ToolUtils.h
    ToolUtils.cpp
)

ADD_MENGINE_LIBRARY()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi)
