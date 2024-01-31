ADD_SUBDIRECTORY(SilentSoundSystem)
ADD_SUBDIRECTORY(MockupRenderSystem)

if(MENGINE_PLATFORM_WIN32)
    if(MENGINE_BUILD_TYPE_DEBUG)
        ADD_SUBDIRECTORY(DebugAllocatorSystem)
    else()
        ADD_SUBDIRECTORY(Win32AllocatorSystem)
    endif()
    
    ADD_SUBDIRECTORY(Win32PreferencesSystem)
    ADD_SUBDIRECTORY(OpenALSoundSystem)
    ADD_SUBDIRECTORY(Win32DateTimeSystem)
    ADD_SUBDIRECTORY(Win32UnicodeSystem)
    ADD_SUBDIRECTORY(Win32ThreadSystem)
    ADD_SUBDIRECTORY(C11TimeSystem)
endif()

if(MENGINE_RENDER_OPENGL)
    ADD_SUBDIRECTORY(OpenGLRenderSystem)
endif()

if(MENGINE_RENDER_DIRECTX9)
    ADD_SUBDIRECTORY(DX9RenderSystem)
endif()

if(MENGINE_RENDER_DIRECTX11)
    ADD_SUBDIRECTORY(DX11RenderSystem)
endif()

if(MENGINE_PLATFORM_SDL)
    ADD_SUBDIRECTORY(POSIXAllocatorSystem)
    ADD_SUBDIRECTORY(OpenALSoundSystem)
    ADD_SUBDIRECTORY(POSIXDateTimeSystem)
    ADD_SUBDIRECTORY(SDLUnicodeSystem)
    ADD_SUBDIRECTORY(SDLThreadSystem)
    ADD_SUBDIRECTORY(C11TimeSystem)

    if(MENGINE_TARGET_WINDOWS)
        ADD_SUBDIRECTORY(Win32PreferencesSystem)
    endif()

    if(MENGINE_TARGET_APPLE)
        ADD_SUBDIRECTORY(ApplePreferencesSystem)
    endif()
    
    if(MENGINE_TARGET_ANDROID)
        ADD_SUBDIRECTORY(AndroidPreferencesSystem)
    endif()
endif()

MENGINE_SUBFOLDER()