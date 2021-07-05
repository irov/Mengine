MENGINE_PROJECT(ImGUIPlugin)

ADD_FILTER(
src
    ImGUIInterface.h

    ImGUIPlugin.cpp
    ImGUIPlugin.h
    ImGUIModule.cpp
    ImGUIModule.h
    
    ImGUIRender.cpp
    ImGUIRender.h
)

if(MENGINE_PLATFORM_WIN32)
    ADD_DEFINITIONS(-DIMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
    
    ADD_FILTER(
    imgui_platform_ext
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_win32.h
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_win32.cpp
    )
endif()

if(MENGINE_PLATFORM_SDL)
    ADD_FILTER(
    imgui_platform_ext
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_sdl.h
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_sdl.cpp
    )

    include_directories(${THIRDPARTY_CONFIG_DIR}/SDL)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)
endif()

if(MENGINE_RENDER_DIRECTX9)
    ADD_FILTER(
    imgui_render_ext
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_dx9.h
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_dx9.cpp
    )
endif()

if(MENGINE_RENDER_OPENGL)
    ADD_FILTER(
    imgui_render_ext
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_opengl2.h
        ${THIRDPARTY_DIR}/imgui/backends/imgui_impl_opengl2.cpp
    )
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui/backends)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IMGUI)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}imgui${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Imm32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Dwmapi.lib)
ENDIF()