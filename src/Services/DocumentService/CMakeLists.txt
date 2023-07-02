MENGINE_PROJECT(DocumentService)

ADD_FILTER(
src
    DocumentService.cpp
    DocumentService.h
    
    Document.cpp
    Document.h
)

ADD_MENGINE_LIBRARY(Services)