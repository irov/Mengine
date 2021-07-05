MENGINE_PROJECT(DazzlePlugin)

ADD_FILTER(
src
    DazzleInterface.h
    
    DazzleIncluder.h

    DazzlePlugin.cpp
    DazzlePlugin.h
    DazzlePlugin.def

    DazzleEffect.cpp
    DazzleEffect.h
    DazzleEffectPrototypeGenerator.cpp
    DazzleEffectPrototypeGenerator.h
    ResourceDazzleEffect.cpp
    ResourceDazzleEffect.h
    ResourceDazzleEffectCustom.cpp
    ResourceDazzleEffectCustom.h
    DataflowDZZ.cpp
    DataflowDZZ.h
    
    DazzleDataInterface.h
    
    DazzleData.cpp
    DazzleData.h
)

IF(NOT MENGINE_BUILD_MENGINE_MASTER_RELEASE)
    ADD_FILTER(
    converter
        DazzleEffectConverterDZBToDZZ.cpp
        DazzleEffectConverterDZBToDZZ.h
    )
ENDIF()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/dazzle/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DAZZLE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}dazzle${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}dazzle_read${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

IF(NOT MENGINE_BUILD_MENGINE_MASTER_RELEASE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}dazzle_write${MENGINE_LIB_SUFFIX})
ENDIF()
