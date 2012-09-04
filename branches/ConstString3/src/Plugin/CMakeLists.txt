ADD_SUBDIRECTORY( MengeImageCodecPlugin )

if(WIN32)
	ADD_SUBDIRECTORY( MengeVideoCodecPlugin )

    if(MSVC)
        ADD_SUBDIRECTORY( MengeSoundCodecPlugin )
        #ADD_SUBDIRECTORY( MengeVideoCodecPlugin )
        
        ADD_SUBDIRECTORY( CURLHTTPSystem )
        ADD_SUBDIRECTORY( MengeHTTPLoggerPlugin )
        ADD_SUBDIRECTORY( MengeXmlCodecPlugin )
        ADD_SUBDIRECTORY( MengeXlsExportPlugin )
        
        ADD_SUBDIRECTORY( ToolsXmlCodecPlugin )
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
