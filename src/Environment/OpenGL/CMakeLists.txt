set(MY_LIB_NAME OpenGL)

ADD_FILTER(
src     
    OpenGLRenderIncluder.h
)

ADD_FILTER(
dummy
    dummy.cpp
)

ADD_MENGINE_LIBRARY(Environment)