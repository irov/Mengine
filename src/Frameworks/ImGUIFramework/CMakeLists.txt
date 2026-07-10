MENGINE_PROJECT(ImGUIFramework)

ADD_FILTER(
src
    ImGUIFrameworkPlugin.cpp
    ImGUIFrameworkPlugin.h
    ImGUIFrameworkPlugin.def

    ImGUIInterface.h

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
    src
        ImGUIWin32Platform.h
        ImGUIWin32Platform.cpp
    )
endif()

if(MENGINE_PLATFORM_SDL2)
    ADD_FILTER(
    src
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_sdl2.h
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_sdl2.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include/include-config-$<LOWER_CASE:$<CONFIG>>)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)
endif()

if(MENGINE_PLATFORM_SDL3)
    ADD_FILTER(
    src
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_sdl3.h
        ${IMGUI_BACKENDS_SOURCE}/imgui_impl_sdl3.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include/include-config-$<LOWER_CASE:$<CONFIG>>)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL3/include)
endif()

if(MENGINE_PLATFORM_MACOS)
    ADD_FILTER(
    src
        ImGUIMacOSPlatform.h
        ImGUIMacOSPlatform.mm
    )
endif()

if(MENGINE_PLATFORM_IOS)
    ADD_FILTER(
    src
        ImGUIiOSPlatform.h
        ImGUIiOSPlatform.mm
    )
endif()

if(MENGINE_RENDER_DIRECTX9)
    ADD_FILTER(
    src
        ImGUIDX9Render.h
        ImGUIDX9Render.cpp
    )
endif()

if(MENGINE_RENDER_OPENGL AND (MENGINE_PLATFORM_MACOS OR MENGINE_PLATFORM_IOS))
    ADD_FILTER(
    src
        ImGUIOpenGL3Render.h
        ImGUIOpenGL3Render.cpp
    )

    if(MENGINE_PLATFORM_IOS)
        ADD_DEFINITIONS(-DIMGUI_IMPL_OPENGL_ES2)
    endif()
elseif(MENGINE_RENDER_OPENGL)
    ADD_FILTER(
    src
        ImGUIOpenGL2Render.h
        ImGUIOpenGL2Render.cpp
    )
elseif(MENGINE_RENDER_METAL)
    ADD_FILTER(
    src
        ImGUIFramebufferDescriptor.h
        ImGUIFramebufferDescriptor.mm
        ImGUIMetalBuffer.h
        ImGUIMetalBuffer.mm
        ImGUIMetalContext.h
        ImGUIMetalContext.mm
        ImGUIMetalRender.h
        ImGUIMetalRender.mm
        ImGUIMetalTexture.h
        ImGUIMetalTexture.mm
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

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IMGUIFRAMEWORK Frameworks)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}imgui${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_PLATFORM_SDL2)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2)
endif()

if(MENGINE_PLATFORM_SDL3)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL3)
endif()

if(MENGINE_PLATFORM_MACOS)
    ADD_MENGINE_APPLE_FRAMEWORK(AppKit)
    ADD_MENGINE_APPLE_FRAMEWORK(GameController)
endif()

if(MENGINE_PLATFORM_IOS)
    ADD_MENGINE_APPLE_FRAMEWORK(UIKit)
endif()

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Imm32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Dwmapi.lib)
ENDIF()
