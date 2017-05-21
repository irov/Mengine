ADD_SUBDIRECTORY( MengeImageCodecPlugin )
ADD_SUBDIRECTORY( MengeSoundCodecPlugin )
ADD_SUBDIRECTORY( MengeAmplifierPlugin )
ADD_SUBDIRECTORY( MengeZipPlugin )
ADD_SUBDIRECTORY( MengeLZ4Plugin )
ADD_SUBDIRECTORY( MengeOggVorbisPlugin )
ADD_SUBDIRECTORY( MengePathFinderPlugin )

ADD_SUBDIRECTORY( Box2DPlugin )
ADD_SUBDIRECTORY( MoviePlugin )
#ADD_SUBDIRECTORY( MengeCal3dPlugin )

if( MENGINE_PLUGIN_ASTRALAX )
    ADD_SUBDIRECTORY( AstralaxParticlePlugin )
endif()

if( MENGINE_PLUGIN_SPINE )
    ADD_SUBDIRECTORY( MengeSpinePlugin )
endif()

if( MENGINE_PLUGIN_STEAM )
    ADD_SUBDIRECTORY( SteamPlugin )
endif()

if( MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( MengeWin32SocketPlugin )    
    ADD_SUBDIRECTORY( MengeVideoCodecPlugin )
    ADD_SUBDIRECTORY( MengeWin32FileGroupPlugin )
    
    ADD_SUBDIRECTORY( GOAPPlugin )
    
    
    #ADD_SUBDIRECTORY( FFMPEGVideoCodecPlugin )
    
    #ADD_SUBDIRECTORY( MengeVideoConverterPlugin )
    #ADD_SUBDIRECTORY( MengeSoundConverterPlugin )
    #ADD_SUBDIRECTORY( MengeHotspotImageConverterPlugin )
    #ADD_SUBDIRECTORY( MengeParticleConverterPlugin )
    #ADD_SUBDIRECTORY( MengeVideoCodecPlugin )

    #ADD_SUBDIRECTORY( CURLHTTPSystem )
    #ADD_SUBDIRECTORY( MengeHTTPLoggerPlugin )
endif()

if( MENGINE_TARGET_MINGW )
    ADD_SUBDIRECTORY( MengeWin32SocketPlugin )
    ADD_SUBDIRECTORY( MengeVideoCodecPlugin )
    ADD_SUBDIRECTORY( MengeWin32FileGroupPlugin )
	ADD_SUBDIRECTORY( Win32ConsoleLogger )
endif()

if( MENGINE_TARGET_MSVC )
    ADD_SUBDIRECTORY( MengeDevelopmentConverterPlugin )
    
    ADD_SUBDIRECTORY( MengeXlsExportPlugin )
    ADD_SUBDIRECTORY( MengeXmlCodecPlugin )
    ADD_SUBDIRECTORY( Win32ConsoleLogger )
endif()

if( MENGINE_TARGET_SDL )
    ADD_SUBDIRECTORY( MengeDevelopmentConverterPlugin )
    
    ADD_SUBDIRECTORY( MengeXlsExportPlugin )
    ADD_SUBDIRECTORY( MengeXmlCodecPlugin )
    ADD_SUBDIRECTORY( Win32ConsoleLogger )
endif()

if( MENGINE_TARGET_SDL )
    ADD_SUBDIRECTORY( MengeVideoCodecPlugin )
    ADD_SUBDIRECTORY( SDLFileGroupPlugin )
endif()

ADD_SUBDIRECTORY( BitmapFontPlugin )

if( MENGINE_PLUGIN_TTF )
    ADD_SUBDIRECTORY( TTFPlugin )
endif()

if( MENGINE_PLUGIN_CURL )
	ADD_SUBDIRECTORY( cURLPlugin )
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
