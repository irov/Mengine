
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