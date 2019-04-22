set( MY_LIB_NAME XmlToBinPlugin )

ADD_FILTER(
src
	XmlToBinPlugin.h
    XmlToBinPlugin.def
	XmlToBinPlugin.cpp
	XmlToBinDecoder.h
	XmlToBinDecoder.cpp    
	XmlToBinInterface.h
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)
add_definitions(-DMENGINE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( 
			         ${THIRDPARTY_DIR}/pugixml/src
                     ${THIRDPARTY_DIR}/metabuf/include
					 ${THIRDPARTY_DIR}/metabuf/src
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
