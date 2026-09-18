MENGINE_PROJECT(MosaicFramework)

ADD_FILTER(
src
    MosaicFrameworkPlugin.cpp
    MosaicFrameworkPlugin.h
    MosaicFrameworkPlugin.def

    MosaicInterface.h

    MosaicService.cpp
    MosaicService.h

    MosaicModule.cpp
    MosaicModule.h

    MosaicRenderPrototypeGenerator.cpp
    MosaicRenderPrototypeGenerator.h

    MosaicRender.cpp
    MosaicRender.h

    MosaicRenderScissor.cpp
    MosaicRenderScissor.h

    MosaicRendererAdapter.cpp
    MosaicRendererAdapter.h

    MosaicFontProvider.cpp
    MosaicFontProvider.h

    MosaicPlatformAdapter.cpp
    MosaicPlatformAdapter.h

    MosaicAllocator.cpp
    MosaicAllocator.h

    MosaicSerializer.cpp
    MosaicSerializer.h

    MosaicKeyCodeTable.cpp
    MosaicKeyCodeTable.h
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        MosaicScriptEmbedding.cpp
        MosaicScriptEmbedding.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(SYSTEM ${THIRDPARTY_DIR}/mosaic/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/graphics/include)

if(NOT MENGINE_PLUGIN_GRAPHICS)
    message(FATAL_ERROR "MosaicFramework requires MENGINE_PLUGIN_GRAPHICS")
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MOSAICFRAMEWORK Frameworks)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}MosaicGraphics${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}MosaicCore${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}graphics${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} GraphicsPlugin)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
