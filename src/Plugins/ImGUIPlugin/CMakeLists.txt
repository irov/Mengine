set(MY_LIB_NAME ImGUIPlugin)

ADD_FILTER(
src
    ImGUIInterface.h

    ImGUIPlugin.cpp
    ImGUIPlugin.h
    ImGUIRender.cpp
    ImGUIRender.h
)

if(MENGINE_PLATFORM_WIN32)
    ADD_FILTER(
    imgui_platform_ext
        ${THIRDPARTY_DIR}/imgui/examples/imgui_impl_win32.h
        ${THIRDPARTY_DIR}/imgui/examples/imgui_impl_win32.cpp
    )
endif()

if(MENGINE_RENDER_DIRECTX)
    ADD_FILTER(
    imgui_render_ext
        ${THIRDPARTY_DIR}/imgui/examples/imgui_impl_dx9.h
        ${THIRDPARTY_DIR}/imgui/examples/imgui_impl_dx9.cpp
    )
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui/examples)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}imgui${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

if(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Xinput.lib)
endif()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)