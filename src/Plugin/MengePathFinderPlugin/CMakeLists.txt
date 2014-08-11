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
    ThreadWorkerPathFinder.cpp
    ThreadWorkerPathFinder.h
    PathGraphNode.h
    PathGraph.h
)

ADD_FILTER(
ModuleAreaOfInterest
    ModuleAreaOfInterest.cpp
	ModuleAreaOfInterest.h
    AreaOfInterest.h
    AreaOfInterest.cpp
    Trigger.cpp
    Trigger.h
)

ADD_FILTER(
Collision
    ModuleCollisionGround.cpp
    ModuleCollisionGround.h
    PythonCollisionRadar.cpp
    PythonCollisionRadar.h
    PythonCollisionObject.cpp
    PythonCollisionObject.h
    CollisionGround.cpp
    CollisionGround.h
    CollisionObject.cpp
    CollisionObject.h
    CollisionRadar.cpp
    CollisionRadar.h
    CollisionMotor.cpp
    CollisionMotor.h
    CollisionMotorFollow.cpp
    CollisionMotorFollow.h
    CollisionMotorPosition.cpp
    CollisionMotorPosition.h
)

INCLUDE_DIRECTORIES(    
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/fastpathfinder/include
    ${THIRDPARTY_DIR}/pybind/include
    )
   
ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

IF(0)
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)

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