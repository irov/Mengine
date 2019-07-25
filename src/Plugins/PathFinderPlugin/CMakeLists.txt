set(MY_LIB_NAME PathFinderPlugin)

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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/Box2D)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/fastpathfinder/include)
   
ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)