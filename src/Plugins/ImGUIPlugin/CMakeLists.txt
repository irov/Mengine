MENGINE_PROJECT(ImGUIPlugin)

ADD_FILTER(
src
    ImGUIInterface.h

    ImGUIPlugin.h
    ImGUIPlugin.def
    ImGUIPlugin.cpp
    
    ImGUIService.cpp
    ImGUIService.h
    
    ImGUIRenderPrototypeGenerator.cpp
    ImGUIRenderPrototypeGenerator.h
    
    ImGUIRenderProvider.cpp
    ImGUIRenderProvider.h
    
    ImGUIRender.cpp
    ImGUIRender.h
)

SET(IMGUI_BACKENDS_SOURCE ${THIRDPARTY_DIR}/imgui/backends)

if(MENGINE_PLATFORM_WIN32)
    ADD_FILTER(
    imgui_platform_ext
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_win32.h
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_win32.cpp
    )
endif()

if(MENGINE_PLATFORM_SDL2)
    ADD_FILTER(
    imgui_platform_ext
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_sdl2.h
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_sdl2.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include/include-config-$<LOWER_CASE:$<CONFIG>>)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2)
endif()

if(MENGINE_RENDER_DIRECTX9)
    ADD_FILTER(
    imgui_render_ext
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_dx9.h
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_dx9.cpp
    )
endif()

if(MENGINE_RENDER_OPENGL)
    ADD_FILTER(
    imgui_render_ext
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_opengl2.h
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_opengl2.cpp
    )
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        ImGUIScriptEmbedding.cpp
        ImGUIScriptEmbedding.h
        
        ImGUIScriptRender.cpp
        ImGUIScriptRender.h
    )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui/backends)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IMGUI)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}imgui${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Imm32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Dwmapi.lib)
ENDIF()