set(MY_LIB_NAME MockupHttpSystem)

ADD_FILTER(
src
	MockupHttpSystem.cpp
	MockupHttpSystem.h
    ThreadTaskDummy.cpp
    ThreadTaskDummy.h
)

ADD_MENGINE_LIBRARY(Systems)