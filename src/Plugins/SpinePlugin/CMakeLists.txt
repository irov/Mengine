set(MY_LIB_NAME SpinePlugin)

ADD_FILTER(
src
    SpineInterface.h

	Spine.h
	Spine.cpp
    
    ResourceSpineSkeleton.h
    ResourceSpineSkeleton.cpp    
    ResourceSpineAtlas.h    
    ResourceSpineAtlasDefault.h
    ResourceSpineAtlasDefault.cpp
    ResourceSpineAtlasTexturepacker.h
    ResourceSpineAtlasTexturepacker.cpp
    
    SpinePlugin.h
    SpinePlugin.cpp
    SpinePlugin.def
    
    LoaderResourceSpineAtlasDefault.h
    LoaderResourceSpineAtlasDefault.cpp
    LoaderResourceSpineAtlasTexturepacker.h
    LoaderResourceSpineAtlasTexturepacker.cpp

    LoaderResourceSpineSkeleton.h
    LoaderResourceSpineSkeleton.cpp    
)

if(MENGINE_USE_PYTHON_FRAMEWORK)
ADD_FILTER(
embedding    
    SpineScriptEmbedding.h
    SpineScriptEmbedding.cpp
    )
    
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()


INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/spine/spine-c/spine-c/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SPINE)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Metacode)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}spine-c${MENGINE_LIB_SUFFIX})
