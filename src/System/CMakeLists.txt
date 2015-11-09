ADD_SUBDIRECTORY( SilentSoundSystem )

if( DEFINED MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( AstralaxParticleSystem2 )
    
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( Win32TimerSystem )
    ADD_SUBDIRECTORY( Win32OptionsSystem )
    #ADD_SUBDIRECTORY( CurlHttpSystem )
    ADD_SUBDIRECTORY( DX9RenderSystem )
endif()

if( DEFINED MENGINE_TARGET_MINGW )
    ADD_SUBDIRECTORY( AstralaxParticleSystem2 )
    
    ADD_SUBDIRECTORY( OALSoundSystem )
    ADD_SUBDIRECTORY( Win32UnicodeSystem )
    ADD_SUBDIRECTORY( Win32ThreadSystem )
    ADD_SUBDIRECTORY( Win32TimerSystem )
    ADD_SUBDIRECTORY( Win32OptionsSystem )
    #ADD_SUBDIRECTORY( CurlHttpSystem )
    ADD_SUBDIRECTORY( DX9RenderSystem )
endif()

if( DEFINED MENGINE_TARGET_SDL )
    ADD_SUBDIRECTORY( AstralaxParticleSystem2 )
    
    ADD_SUBDIRECTORY( SDLSoundSystem )
    ADD_SUBDIRECTORY( PosixUnicodeSystem )
    ADD_SUBDIRECTORY( SDLThreadSystem )
    
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
