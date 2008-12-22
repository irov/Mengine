
ADD_FILTER(
src
	XmlElement.cpp
	XmlElement.h
	XmlElementListener.cpp
	XmlElementListener.h
	XmlElementValueListener.h
	XmlExpatParser.cpp
	XmlExpatParser.h
	XmlParser.cpp
	XmlParser.h
	XmlParserCast.cpp
	XmlParserCast.h
	XmlParserCheckMethod.h
	XmlParserElement.cpp
	XmlParserElement.h
)


ADD_LIBRARY( XmlParser ${SRC_FILES} )
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 XmlParser
)

SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

#IF( WIN32 )
#	SET_PROPERTY( TARGET XmlParser APPEND PROPERTY LINK_FLAG_DEBUG /LIBPATH:"${Mengine_SOURCE_DIR}/../dependencies/expat/win32/debug" )
#	SET_PROPERTY( TARGET XmlParser APPEND PROPERTY LINK_FLAG_RELEASE /LIBPATH:"${Mengine_SOURCE_DIR}/../dependencies/expat/win32/release" )
#ENDIF( WIN32 )

#IF( MSVC )
#	TARGET_LINK_LIBRARIES( XmlParser libexpatMT.lib )
#ELSE( MSVC )
#	TARGET_LING_LIBRARIES( XmlParser libexpat.a )
#ENDIF( MSVC )