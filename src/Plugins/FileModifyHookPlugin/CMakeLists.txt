MENGINE_PROJECT(FileModifyHookPlugin)

ADD_FILTER(
src
    FileModifyHookPlugin.h
    FileModifyHookPlugin.def
    FileModifyHookPlugin.cpp
    
    FileModifyHookInterface.h
    
    FileModifyHookService.cpp
    FileModifyHookService.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_FILEMODIFYHOOK)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)