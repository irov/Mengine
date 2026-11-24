MENGINE_PROJECT(InputService)

ADD_FILTER(
src
    InputService.cpp
    InputService.h
    TouchGestureRecognizer.cpp
    TouchGestureRecognizer.h
)

ADD_MENGINE_LIBRARY(Services)