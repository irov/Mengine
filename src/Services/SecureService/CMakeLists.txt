MENGINE_PROJECT(SecureService)

ADD_FILTER(
src
    SecureService.h
    SecureService.cpp
    
    SecureUnsignedValue.h
    SecureUnsignedValue.cpp
    SecureStringValue.h
    SecureStringValue.cpp    
)

ADD_MENGINE_LIBRARY(Services)