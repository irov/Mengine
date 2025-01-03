MENGINE_PROJECT(AreaOfInterestPlugin)

ADD_FILTER(
src
    AreaOfInterestPlugin.h
    AreaOfInterestPlugin.cpp
    AreaOfInterestPlugin.def
    
    AreaOfInterestInterface.h

    AreaOfInterestService.cpp
    AreaOfInterestService.h
    AreaOfInterestZone.cpp
    AreaOfInterestZone.h 
    NodeAreaOfInterestTrigger.cpp
    NodeAreaOfInterestTrigger.h
    NodeAreaOfInterestTriggerEventReceiverInterface.h
    NodeAreaOfInterestActor.cpp
    NodeAreaOfInterestActor.h
    AreaOfInterestTrigger.cpp
    AreaOfInterestTrigger.h
    AreaOfInterestActor.cpp
    AreaOfInterestActor.h
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AreaOfInterestScriptEmbedding.cpp
        AreaOfInterestScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_AREAOFINTEREST)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)