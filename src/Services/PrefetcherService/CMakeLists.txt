MENGINE_PROJECT(PrefetcherService)

ADD_FILTER(
src
    PrefetcherService.h
    PrefetcherService.cpp
    PrefetchReceiver.h
    PrefetchReceiver.cpp
    ThreadTaskPrefetch.h
    ThreadTaskPrefetch.cpp
    ThreadTaskPrefetchDataflow.h
    ThreadTaskPrefetchDataflow.cpp
    ThreadTaskPrefetchImageDecoder.h
    ThreadTaskPrefetchImageDecoder.cpp
    ThreadTaskPrefetchSoundDecoder.h
    ThreadTaskPrefetchSoundDecoder.cpp
    ThreadTaskPrefetchStream.h
    ThreadTaskPrefetchStream.cpp
)

ADD_MENGINE_LIBRARY(Services)