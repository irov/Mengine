set( MY_LIB_NAME ToolsXmlCodecPlugin )

ADD_FILTER(
src
	XmlToBinDecoder.h
	XmlToBinDecoder.cpp
    XmlCodecPlugin.def
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils
			         ${THIRDPARTY_DIR}/pugixml/src
                     ${THIRDPARTY_DIR}/metabuf
)
 
ADD_LIBRARY( ${MY_LIB_NAME} SHARED  ${SRC_FILES} )

SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core )

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}pugixml_static
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}xml2metabuf_static
    )

#ADD_EXECUTABLE( ${MY_LIB_NAME} ${SRC_FILES} )