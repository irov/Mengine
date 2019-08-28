set(MY_LIB_NAME ResourcePrefetcherPlugin)

ADD_FILTER(
src
    ResourcePrefetcherInterface.h
    ResourcePrefetcherServiceInterface.h

	ResourcePrefetcherPlugin.h
    ResourcePrefetcherPlugin.def
	ResourcePrefetcherPlugin.cpp
    
    ResourcePrefetcherService.h
    ResourcePrefetcherService.cpp
    
    BaseResourcePrefetcher.h
    BaseResourcePrefetcher.cpp
    
    DefaultResourcePrefetcher.h
    DefaultResourcePrefetcher.cpp
    DataflowResourcePrefetcher.h
    DataflowResourcePrefetcher.cpp
    ArchiveResourcePrefetcher.h
    ArchiveResourcePrefetcher.cpp
    ImageDecoderResourcePrefetcher.h
    ImageDecoderResourcePrefetcher.cpp
    SoundDecoderResourcePrefetcher.h
    SoundDecoderResourcePrefetcher.cpp    
)

if(MENGINE_USE_PYTHON_FRAMEWORK)
    ADD_FILTER(
    embedding
        ResourcePrefetcherScriptEmbedding.cpp
        ResourcePrefetcherScriptEmbedding.h
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)