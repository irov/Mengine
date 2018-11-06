ADD_SUBDIRECTORY( SilentSoundSystem )

if( MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( OpenALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( C11TimeSystem )
    ADD_SUBDIRECTORY( Win32PluginSystem )    
    ADD_SUBDIRECTORY( DX9RenderSystem )
endif()

if( MENGINE_TARGET_MINGW )
    ADD_SUBDIRECTORY( OpenALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( C11TimeSystem )
    ADD_SUBDIRECTORY( Win32PluginSystem )
    ADD_SUBDIRECTORY( DX9RenderSystem )
endif()

if( MENGINE_TARGET_SDL )
    ADD_SUBDIRECTORY( OpenALSoundSystem )
    ADD_SUBDIRECTORY( SDLUnicodeSystem )
    ADD_SUBDIRECTORY( SDLThreadSystem )
    ADD_SUBDIRECTORY( C11TimeSystem )
    ADD_SUBDIRECTORY( SDLPluginSystem )
    
    if( MENGINE_PLATFORM_IOS )
        ADD_SUBDIRECTORY( MockupSocketSystem )
    endif()

    ADD_SUBDIRECTORY( OpenGLRenderSystem )
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
