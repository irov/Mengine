MENGINE_PROJECT(Android)

ADD_FILTER(
src
    AndroidIncluder.h
    AndroidEventation.h
    AndroidEventationInterface.h
    AndroidEventationHub.h
    AndroidEventationHub.cpp
    AndroidUtils.h
    AndroidUtils.cpp

        AndroidEnvironmentService.h
        AndroidEnvironmentService.cpp
)

ADD_MENGINE_LIBRARY(Environment)