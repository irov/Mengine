ADD_SUBDIRECTORY( ImageCodecPlugin )
ADD_SUBDIRECTORY( SoundCodecPlugin )
ADD_SUBDIRECTORY( AmplifierPlugin )
ADD_SUBDIRECTORY( ZipPlugin )
ADD_SUBDIRECTORY( LZ4Plugin )
ADD_SUBDIRECTORY( OggVorbisPlugin )
ADD_SUBDIRECTORY( PathFinderPlugin )

ADD_SUBDIRECTORY( Box2DPlugin )
ADD_SUBDIRECTORY( MoviePlugin )

if( MENGINE_PLUGIN_ASTRALAX )
    ADD_SUBDIRECTORY( AstralaxParticlePlugin )
endif()

if( MENGINE_PLUGIN_SPINE )
    ADD_SUBDIRECTORY( SpinePlugin )
endif()

if( MENGINE_PLUGIN_STEAM )
    ADD_SUBDIRECTORY( SteamPlugin )
endif()

if( MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( Win32SocketPlugin )    
    ADD_SUBDIRECTORY( VideoCodecPlugin )
    ADD_SUBDIRECTORY( Win32FileGroupPlugin )
    
    #ADD_SUBDIRECTORY( GOAPPlugin )
    
    
    #ADD_SUBDIRECTORY( FFMPEGVideoCodecPlugin )
    
    #ADD_SUBDIRECTORY( VideoConverterPlugin )
    #ADD_SUBDIRECTORY( SoundConverterPlugin )
    #ADD_SUBDIRECTORY( HotspotImageConverterPlugin )
    #ADD_SUBDIRECTORY( ParticleConverterPlugin )
    #ADD_SUBDIRECTORY( VideoCodecPlugin )

    #ADD_SUBDIRECTORY( HTTPLoggerPlugin )
endif()

if( MENGINE_TARGET_MINGW )
    ADD_SUBDIRECTORY( Win32SocketPlugin )
    ADD_SUBDIRECTORY( VideoCodecPlugin )
    ADD_SUBDIRECTORY( Win32FileGroupPlugin )
	ADD_SUBDIRECTORY( Win32ConsoleLogger )
endif()

if( MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( DevelopmentConverterPlugin )
    
    ADD_SUBDIRECTORY( XlsExportPlugin )
    ADD_SUBDIRECTORY( XmlCodecPlugin )
    ADD_SUBDIRECTORY( Win32ConsoleLogger )
endif()

if( MENGINE_TARGET_SDL AND MENGINE_PLATFORM_WIN32 )
    ADD_SUBDIRECTORY( DevelopmentConverterPlugin )
    
    ADD_SUBDIRECTORY( XlsExportPlugin )
    ADD_SUBDIRECTORY( XmlCodecPlugin )
    ADD_SUBDIRECTORY( Win32ConsoleLogger )
endif()

if( MENGINE_TARGET_SDL )
    ADD_SUBDIRECTORY( VideoCodecPlugin )
    ADD_SUBDIRECTORY( SDLFileGroupPlugin )
endif()

ADD_SUBDIRECTORY( BitmapFontPlugin )

if( MENGINE_PLUGIN_TTF )
    ADD_SUBDIRECTORY( TTFPlugin )
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
