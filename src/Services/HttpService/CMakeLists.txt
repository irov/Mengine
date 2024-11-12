MENGINE_PROJECT(HttpService)

ADD_FILTER(
src
    HttpService.h
    HttpService.cpp
    
    HttpResponse.h
    HttpResponse.cpp
    
    HttpReceiverPing.h
    HttpReceiverPing.cpp
)

ADD_MENGINE_LIBRARY(Services)