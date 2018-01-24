ADD_SUBDIRECTORY( SilentSoundSystem )

if( MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( C11TimerSystem )
    ADD_SUBDIRECTORY( Win32PluginSystem )    
    ADD_SUBDIRECTORY( DX9RenderSystem )
endif()

if( MENGINE_TARGET_MINGW )
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( C11TimerSystem )
    ADD_SUBDIRECTORY( Win32PluginSystem )
    ADD_SUBDIRECTORY( DX9RenderSystem )
endif()

if( MENGINE_TARGET_SDL )
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( SDLUnicodeSystem )
    ADD_SUBDIRECTORY( SDLThreadSystem )
    ADD_SUBDIRECTORY( C11TimerSystem )
    ADD_SUBDIRECTORY( SDLPluginSystem )

    if( MENGINE_PLATFORM_MAC OR MENGINE_PLATFORM_WIN32 )
        ADD_SUBDIRECTORY( SDLSocketSystem )
    endif()    
    
    if( MENGINE_PLATFORM_IOS )
        ADD_SUBDIRECTORY( MockupSocketSystem )
    endif()

    if( MENGINE_PLATFORM_IOS )
        ADD_SUBDIRECTORY( OpenGLRenderSystemES )
    else()
        ADD_SUBDIRECTORY( OpenGLRenderSystem )
    endif()    
endif()

if( MENGINE_PLUGIN_CURL )
    ADD_SUBDIRECTORY( cURLHttpSystem )
else()
    ADD_SUBDIRECTORY( MockupHttpSystem )
endif()

SET( APPLICATION_DEPENDENCIES
    ${APPLICATION_DEPENDENCIES}
    PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
    ${APPLICATION_LIB_PATHS}
    PARENT_SCOPE
)
