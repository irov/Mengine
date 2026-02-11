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

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AreaOfInterestScriptEmbedding.cpp
        AreaOfInterestScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_AREAOFINTEREST)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)