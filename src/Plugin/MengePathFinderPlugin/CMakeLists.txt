set( MY_LIB_NAME MengePathFinderPlugin )

ADD_FILTER(
ModulePathFinder
    PluginPathFinder.cpp
    PluginPathFinder.h
	ModulePathFinder.cpp
	ModulePathFinder.h
	PathFinderMap.cpp
	PathFinderMap.h
	PathFinder.h
    PathFinder.cpp
	PathFinderPool.cpp
	PathFinderPool.h
    PathFinderWay.cpp
    PathFinderWay.h
    PathFinderWayAffector.cpp
    PathFinderWayAffector.h
    ThreadWorkerPathFinder.cpp
    ThreadWorkerPathFinder.h
    PathGraphNode.h
    PathGraph.h
)

ADD_FILTER(
ModuleBurritoWorld
    ModuleBurritoWorld.cpp
	ModuleBurritoWorld.h
    BurritoWorld.h
    BurritoWorld.cpp
    BurritoBison.h
    BurritoBison.cpp
    BurritoGround.h
    BurritoGround.cpp
    BurritoUnit.h
    BurritoUnit.cpp
    BurritoCameraTarget.h
    BurritoCameraTarget.cpp
	Endless.cpp
	Endless.h    
)

ADD_FILTER(
ModuleAreaOfInterest
    ModuleAreaOfInterest.cpp
	ModuleAreaOfInterest.h
    AOIInterface.h
    AreaOfInterest.h
    AreaOfInterest.cpp
    AOITrigger.cpp
    AOITrigger.h
    AOIActor.cpp
    AOIActor.h  
    NodeAOITrigger.h
    NodeAOITrigger.cpp
    NodeAOIActor.h
    NodeAOIActor.cpp
)

ADD_FILTER(
ModuleCollision
    ModuleCollision.cpp
    ModuleCollision.h
    CollisionWorld.cpp
    CollisionWorld.h
    CollisionActor.cpp
    CollisionActor.h
    NodeCollisionActor.cpp
    NodeCollisionActor.h
)

ADD_FILTER(
ModuleMagnet
    ModuleMagnet.cpp
    ModuleMagnet.h    
    MagnetWorld.cpp
    MagnetWorld.h
    MagnetActor.cpp
    MagnetActor.h
    NodeMagnetActor.cpp
    NodeMagnetActor.h
)

ADD_FILTER(
ModuleBounce
    ModuleBounce.cpp
    ModuleBounce.h    
    BounceWorld.cpp
    BounceWorld.h
    BounceActor.cpp
    BounceActor.h
)

ADD_FILTER(
ModuleInvader   
    ModuleInvader.cpp
    ModuleInvader.h
    InvaderFollowAffector.cpp
    InvaderFollowAffector.h
)

INCLUDE_DIRECTORIES(    
    ${Mengine_SOURCE_DIR}/Utils     
    ${THIRDPARTY_DIR}/metabuf
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/fastpathfinder/include
    )
   
ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

IF(0)
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Math)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Metacode)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX} )
ENDIF(0)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS} 
 PARENT_SCOPE
)