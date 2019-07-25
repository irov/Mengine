set(MY_LIB_NAME Windows)

ADD_FILTER(
src     
    WindowsIncluder.h
    
)

ADD_FILTER(
dummy
	dummy.cpp
)

ADD_MENGINE_LIBRARY(Environment)