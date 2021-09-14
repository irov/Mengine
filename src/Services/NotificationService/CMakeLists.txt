MENGINE_PROJECT(NotificationService)

ADD_FILTER(
src
    NotificationService.h
    NotificationService.cpp
    
    NotificationArea.h
    NotificationArea.cpp
)

ADD_MENGINE_LIBRARY(Services)