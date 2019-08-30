set(MY_LIB_NAME VideoPlugin)

ADD_FILTER(
src
	VideoInterface.h
	VideoUnknownInterface.h

	VideoPlugin.h
	VideoPlugin.cpp
    VideoPlugin.def
    
    ResourceVideo.cpp
    ResourceVideo.h
    
    LoaderResourceVideo.cpp
    LoaderResourceVideo.h    
	
	SurfaceVideo.cpp
	SurfaceVideo.h
	
	ResourceVideoValidator.cpp
	ResourceVideoValidator.h
	)
    
if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding    
        VideoScriptEmbedding.cpp
        VideoScriptEmbedding.h
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)    
endif()    
    
add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_VIDEO)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${VIDEO_CODEC_STATIC_LIBS})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)