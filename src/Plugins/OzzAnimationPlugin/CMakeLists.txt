MENGINE_PROJECT(OzzAnimationPlugin)

ADD_FILTER(
src
    OzzAnimationPlugin.cpp
    OzzAnimationPlugin.h
    OzzAnimationPlugin.def
    
    OzzAnimationInterface.h

    OzzDetail.cpp
    OzzDetail.h
    ResourceOzzSkeleton.cpp
    ResourceOzzSkeleton.h
    ResourceOzzMesh.cpp
    ResourceOzzMesh.h
    ResourceOzzAnimation.cpp
    ResourceOzzAnimation.h
    SamplerOzzAnimation.cpp
    SamplerOzzAnimation.h
    NodeOzzAnimation.cpp
    NodeOzzAnimation.h
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        OzzScriptEmbedding.cpp
        OzzScriptEmbedding.h
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)    
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/ozz/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_OZZ_ANIMATION)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ozz_base${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ozz_options${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ozz_geometry${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ozz_animation${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ozz_animation_offline${MENGINE_LIB_SUFFIX})

if(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zsd${MENGINE_LIB_SUFFIX})
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}z${MENGINE_LIB_SUFFIX})
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)