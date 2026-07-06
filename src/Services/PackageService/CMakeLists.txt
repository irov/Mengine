MENGINE_PROJECT(PackageService)

ADD_FILTER(
src
    PackageService.h
    PackageService.cpp
    Package.h
    Package.cpp
)

ADD_MENGINE_LIBRARY(Services)
