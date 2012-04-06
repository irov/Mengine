set( MY_LIB_NAME MengeXmlCodecPlugin )

ADD_FILTER(
src
	XmlCodecPlugin.h
	XmlCodecPlugin.cpp
	XmlToBinDecoder.h
	XmlToBinDecoder.cpp
    XmlToBin.cpp
    XmlToBin.h 
    XmlCodecPlugin.def
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils
			         ${THIRDPARTY_DIR}/pugixml/src
)

#MESSAGE( STATUS "CMAKE_CURRENT_LIB_DIR: " ${MENGINE_LIB_DIR} )
#set this for linking dll with libs ( with using TARGET_LINK_LIBRARIES )
SET(TINYXML_LIB_DIR  ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/Debug/ )

SET(
 TINYXML_STATIC_LIBS
 debug  ${TINYXML_LIB_DIR}${MENGINE_LIB_PREFIX}pugixml_static${MENGINE_LIB_SUFFIXD}
 optimized  ${TINYXML_LIB_DIR}${MENGINE_LIB_PREFIX}pugixml_static${MENGINE_LIB_SUFFIX}
 )
 
#ADD_DEPENDENCIES( ${MY_LIB_NAME} LoggerLib )
				 
ADD_LIBRARY( ${MY_LIB_NAME} SHARED  ${SRC_FILES} )

#SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS "/def:XmlCodecPlugin.def" )

SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${TINYXML_STATIC_LIBS} )

#ADD_EXECUTABLE( ${MY_LIB_NAME} ${SRC_FILES} )