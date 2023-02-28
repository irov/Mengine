MENGINE_PROJECT(Apple)

ADD_FILTER(
src
    AppleUtils.h
    AppleUtils.mm
    AppleDetail.h
    AppleDetail.mm
    AppleErrorHelper.h
    AppleErrorHelper.mm
    AppleString.h
    AppleString.mm
)

ADD_MENGINE_LIBRARY(Environment)