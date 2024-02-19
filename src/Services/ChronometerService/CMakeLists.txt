MENGINE_PROJECT(ChronometerService)

ADD_FILTER(
src
    ChronometerService.h
    ChronometerService.cpp
    
    Chronometer.h
    Chronometer.cpp
)

ADD_MENGINE_LIBRARY(Services)