MENGINE_PROJECT(LoggerService)

ADD_FILTER(
src
    LoggerService.h
    LoggerService.cpp
    
    LoggerRecord.h
    LoggerRecord.cpp
)

ADD_MENGINE_LIBRARY(Services)