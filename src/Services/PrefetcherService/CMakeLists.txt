SET(MY_LIB_NAME PrefetcherService)

ADD_FILTER(
src
    PrefetcherService.h
	PrefetcherService.cpp    
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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)