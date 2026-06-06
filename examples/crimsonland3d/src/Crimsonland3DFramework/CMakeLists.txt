MENGINE_PROJECT(Crimsonland3DGame)

ADD_FILTER(
Crimsonland3DGame
    Crimsonland3DGame.cpp
    Crimsonland3DGame.h
    Crimsonland3DGamePlugin.cpp
    Crimsonland3DGamePlugin.h
    Crimsonland3DSceneEventReceiver.cpp
    Crimsonland3DSceneEventReceiver.h
    Crimsonland3DDungeonGenerator.cpp
    Crimsonland3DDungeonGenerator.h
    Crimsonland3DCombat.cpp
    Crimsonland3DCombat.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_EXAMPLEFRAMEWORK Game OFF)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER Game
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)

# Expose the GLTF importer service interface header (plugin is linked via ENABLE_PLUGIN).
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${MENGINE_SOURCE_DIR}/Plugins/GLTFImporterPlugin)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
