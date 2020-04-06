MENGINE_PROJECT(ThreadService)

ADD_FILTER(
src
    ThreadService.h
    ThreadService.cpp
    ThreadQueue.h
    ThreadQueue.cpp
)

ADD_MENGINE_LIBRARY(Services)