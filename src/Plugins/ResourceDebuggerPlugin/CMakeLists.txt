SET(MY_LIB_NAME ResourceDebuggerPlugin)

ADD_FILTER(
src
    ResourceDebuggerPlugin.h
    ResourceDebuggerPlugin.cpp
    ResourceDebuggerPlugin.def
    ResourceUselessCompileChecker.h
    ResourceUselessCompileChecker.cpp
)

ADD_MENGINE_LIBRARY(Plugins)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)