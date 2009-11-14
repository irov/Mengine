
set( MY_LIB_NAME XmlParser )

if( MENGE_USE_MT )
	set( MY_LIB_NAME ${MY_LIB_NAME}_mt )
endif( MENGE_USE_MT )

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

ADD_DEFINITIONS( -DXML_STATIC )

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/expat/lib )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 debug ${MENGINE_LIB_PREFIX}expat_static${MENGINE_LIB_SUFFIXD}
 optimized ${MENGINE_LIB_PREFIX}expat_static${MENGINE_LIB_SUFFIX}
)

 
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
