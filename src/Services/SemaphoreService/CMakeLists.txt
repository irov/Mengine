MENGINE_PROJECT(SemaphoreService)

ADD_FILTER(
src
    Semaphore.h
    Semaphore.cpp
    
    SemaphoreService.h
    SemaphoreService.cpp
)

ADD_MENGINE_LIBRARY(Services)