MENGINE_PROJECT(SheenBidiPlugin)

ADD_FILTER(
src
    SheenBidiPlugin.h
    SheenBidiPlugin.cpp
    SheenBidiPlugin.def
    SheenBidiTextAlgorithm.h
    SheenBidiTextAlgorithm.cpp
    SheenBidiTextBidirectional.h
    SheenBidiTextBidirectional.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/sheenbidi/Headers)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SHEENBIDI)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}sheenbidi${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
