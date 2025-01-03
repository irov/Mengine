MENGINE_PROJECT(ResourcePrefetcherPlugin)

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

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        ResourcePrefetcherScriptEmbedding.cpp
        ResourcePrefetcherScriptEmbedding.h
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_RESOURCEPREFETCHER)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)