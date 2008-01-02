#	include "XmlExpatParser.h"
#	include <stdio.h>

#	include "XmlElement.h"
#	include "XmlListener.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbBeginElement( void *userData, const char *name, const char **atts )
{
	XmlExpatParser * parser = static_cast<XmlExpatParser*>( userData );
	XmlListener * parser_listener = parser->topListener();

	XmlElement element( parser, name, atts );

	parser_listener->parseXML( &element );

	XmlListener * incref_listener = parser->topListener();
	incref_listener->incref();
}
//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbEndElement( void *userData, const char *name )
{
	XmlExpatParser * parser = static_cast<XmlExpatParser*>( userData );
	XmlListener * listener = parser->topListener();
	if( listener->decref() )
	{
		parser->popListener();
	}
}
//////////////////////////////////////////////////////////////////////////
XmlExpatParser::XmlExpatParser()
{
	m_parser = XML_ParserCreate(NULL);
}
//////////////////////////////////////////////////////////////////////////
XmlExpatParser::~XmlExpatParser()
{
	XML_ParserFree( m_parser );
}
//////////////////////////////////////////////////////////////////////////
void * XmlExpatParser::makeBuffer( int _size )
{
	return XML_GetBuffer( m_parser, _size );
}
//////////////////////////////////////////////////////////////////////////
bool XmlExpatParser::parseXML( int _size, XmlListener * _listener )
{
	XML_SetUserData( m_parser, this );
	XML_SetElementHandler( m_parser, cbBeginElement, cbEndElement );

	pushListener( _listener );

	bool done = true;

	if( XML_ParseBuffer( m_parser, _size, XML_TRUE ) == XML_STATUS_ERROR )
	{
		fprintf(stderr,
			"%s at line %" XML_FMT_INT_MOD "u\n",
			XML_ErrorString(XML_GetErrorCode(m_parser)),
			XML_GetCurrentLineNumber(m_parser)
			);

		done = false;
	}

	clearListener();
	XML_ParserReset( m_parser, NULL );

	return done;
}
//////////////////////////////////////////////////////////////////////////
void XmlExpatParser::pushListener( XmlListener * _listener )
{
	m_listStackListener.push_back( _listener );
}
//////////////////////////////////////////////////////////////////////////
XmlListener * XmlExpatParser::topListener()
{
	return m_listStackListener.back();
}
//////////////////////////////////////////////////////////////////////////
void XmlExpatParser::popListener()
{
	delete m_listStackListener.back();
	m_listStackListener.pop_back();
}
//////////////////////////////////////////////////////////////////////////
void XmlExpatParser::clearListener()
{
	for( TListStackListener::iterator
		it = m_listStackListener.begin(),
		it_end = m_listStackListener.end();
	it != it_end;
	++it)
	{
		delete *it;
	}

	m_listStackListener.clear();
}