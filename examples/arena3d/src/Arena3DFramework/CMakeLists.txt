MENGINE_PROJECT(Arena3DGame)

ADD_FILTER(
Arena3DGame
    Arena3DHud.cpp
    Arena3DHud.h
    Arena3DHelper.cpp
    Arena3DHelper.h
    Arena3DGame.cpp
    Arena3DGame.h
    Arena3DGamePlugin.cpp
    Arena3DGamePlugin.h
    Arena3DSceneEventReceiver.cpp
    Arena3DSceneEventReceiverEffects.cpp
    Arena3DSceneEventReceiver.h
    Arena3DSceneEventReceiverPresentation.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_EXAMPLEFRAMEWORK Game OFF)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Game)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel Engine Arena3DSimulation Arena3DNetworking)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${MENGINE_SOURCE_DIR}/Plugins/GLTFImporterPlugin)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/Data/generated/include)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
ADD_DEPENDENCIES(${PROJECT_NAME} Arena3DContent)
