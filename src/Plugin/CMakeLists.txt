ADD_SUBDIRECTORY( MengeImageCodecPlugin )
ADD_SUBDIRECTORY( MengeSoundCodecPlugin )
ADD_SUBDIRECTORY( MengeAmplifierPlugin )
ADD_SUBDIRECTORY( MengeZipPlugin )
ADD_SUBDIRECTORY( MengeLZ4Plugin )
ADD_SUBDIRECTORY( MengePathFinderPlugin )
#ADD_SUBDIRECTORY( MengeCal3dPlugin )

if(WIN32)    
	ADD_SUBDIRECTORY( MengeVideoCodecPlugin )
    ADD_SUBDIRECTORY( MengeWin32FileGroupPlugin )
    ADD_SUBDIRECTORY( FFMPEGVideoCodecPlugin )
    
    if(MSVC)        
        ADD_SUBDIRECTORY( MengeDevelopmentConverterPlugin )        
        #ADD_SUBDIRECTORY( MengeVideoConverterPlugin )
        #ADD_SUBDIRECTORY( MengeSoundConverterPlugin )        
        #ADD_SUBDIRECTORY( MengeHotspotImageConverterPlugin )
        #ADD_SUBDIRECTORY( MengeParticleConverterPlugin )
        #ADD_SUBDIRECTORY( MengeVideoCodecPlugin )
        
        #ADD_SUBDIRECTORY( CURLHTTPSystem )
        #ADD_SUBDIRECTORY( MengeHTTPLoggerPlugin )
        
        ADD_SUBDIRECTORY( MengeXlsExportPlugin )
        ADD_SUBDIRECTORY( MengeXmlCodecPlugin )
        
#        ADD_SUBDIRECTORY( ToolsHotspotImageConverterPlugin )
#        ADD_SUBDIRECTORY( ToolsXmlCodecPlugin )
    endif(MSVC)    
endif(WIN32)

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
