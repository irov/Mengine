set( MY_LIB_NAME XmlCodecPlugin )

ADD_FILTER(
src
	XmlCodecPlugin.h
	XmlCodecPlugin.cpp
	XmlToBinDecoder.h
	XmlToBinDecoder.cpp
    XmlCodecPlugin.def
)

add_definitions(-DMENGINE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils
			         ${THIRDPARTY_DIR}/pugixml/src
                     ${THIRDPARTY_DIR}/metabuf
                     ${THIRDPARTY_DIR}/utf8/source
                     ${THIRDPARTY_DIR}/libmath/include
)
		 
ADD_MENGINE_SHARED( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel )

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}    
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}xml2metabuf${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pugixml${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
    )
