set( MY_LIB_NAME MengeDevelopmentConverterPlugin )

ADD_FILTER(
src
	DevelopmentConverter.cpp
    DevelopmentConverter.h
	DevelopmentConverterPlugin.cpp
    DevelopmentConverterPlugin.def
    DevelopmentConverterPlugin.h
    
	HotspotImageConverterPNGToHIT.cpp
    HotspotImageConverterPNGToHIT.h
    SoundConverterFFMPEGToOGG.cpp
	SoundConverterFFMPEGToOGG.h
	VideoConverterFFMPEGToWEBM.cpp
	VideoConverterFFMPEGToWEBM.h
	ParticleConverterPTCToPTZ.cpp
	ParticleConverterPTCToPTZ.h    
    MovieKeyConverterXMLToAEK.cpp
    MovieKeyConverterXMLToAEK.h
    ModelConverterMDLToMDZ.cpp
    ModelConverterMDLToMDZ.h
)

INCLUDE_DIRECTORIES(
			${Mengine_SOURCE_DIR}/Utils    
            ${THIRDPARTY_DIR}/metabuf            
	)


ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

#SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Metacode )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}metabuf_static${MENGINE_LIB_SUFFIX})

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS} 
 PARENT_SCOPE
)