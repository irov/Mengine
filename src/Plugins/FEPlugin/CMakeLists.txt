MENGINE_PROJECT(FEPlugin)

ADD_FILTER(
src
    FEPlugin.h
    FEPlugin.cpp
    FEPlugin.def
    
    FEServiceInterface.h
    FEService.h
    FEService.cpp

    FEFontEffectBase.h
    FEFontEffectBase.cpp
    FEFontEffectFile.h
    FEFontEffectFile.cpp
    FEFontEffectCustom.h
    FEFontEffectCustom.cpp
    FEInterface.h
    FEDataInterface.h
    FEData.h
    FEData.cpp
    FEDataflow.h
    FEDataflow.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libfe/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_FE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}font-effects${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)