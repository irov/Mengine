ADD_SUBDIRECTORY( SilentSoundSystem )

if( MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( Win32TimerSystem )    
    ADD_SUBDIRECTORY( Win32PluginSystem )    
    ADD_SUBDIRECTORY( DX9RenderSystem )
    ADD_SUBDIRECTORY( cURLHttpSystem )
endif()

if( MENGINE_TARGET_MINGW )
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( Win32TimerSystem )    
    ADD_SUBDIRECTORY( Win32PluginSystem )    
    ADD_SUBDIRECTORY( DX9RenderSystem )
    #ADD_SUBDIRECTORY( cURLHttpSystem )
    ADD_SUBDIRECTORY( MockupHttpSystem )
endif()

if( MENGINE_TARGET_SDL )
    #ADD_SUBDIRECTORY( SDLSoundSystem )
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( SDLUnicodeSystem )
    ADD_SUBDIRECTORY( SDLThreadSystem )
    ADD_SUBDIRECTORY( SDLTimerSystem )    
    ADD_SUBDIRECTORY( SDLPluginSystem )    
    ADD_SUBDIRECTORY( SDLSocketSystem )
    ADD_SUBDIRECTORY( OpenGLRenderSystem )
if( MENGINE_PLATFORM_WIN32 )
	ADD_SUBDIRECTORY( cURLHttpSystem )
endif()
if( MENGINE_PLATFORM_XCODE )
    ADD_SUBDIRECTORY( MockupHttpSystem )
endif()    
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
