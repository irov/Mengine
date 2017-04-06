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
    VideoConverterFFMPEGToOGV.cpp
    VideoConverterFFMPEGToOGV.h
    VideoConverterFFMPEGToOGVA.cpp
    VideoConverterFFMPEGToOGVA.h
	ParticleConverterPTCToPTZ.cpp
	ParticleConverterPTCToPTZ.h    
    MovieKeyConverterXMLToAEK.cpp
    MovieKeyConverterXMLToAEK.h
    ModelConverterMDLToMDZ.cpp
    ModelConverterMDLToMDZ.h
    ImageConverterPVRToHTF.cpp
    ImageConverterPVRToHTF.h
    ImageConverterDDSToHTF.cpp
    ImageConverterDDSToHTF.h
    ImageConverterPNGToACF.cpp
    ImageConverterPNGToACF.h
)

add_definitions(-DMENGE_PLUGIN_DLL)

INCLUDE_DIRECTORIES(
			${Mengine_SOURCE_DIR}/Utils    
            ${THIRDPARTY_DIR}/pybind/include
            ${THIRDPARTY_DIR}/metabuf 
            ${THIRDPARTY_DIR}/poly2tri
	)
    

ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Math )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Metacode )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}lz4${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}poly2tri${MENGINE_LIB_SUFFIX})