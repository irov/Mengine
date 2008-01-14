#	include "XmlExpatParser.h"
#	include <stdio.h>

#	include "expat.h"

#	include "XmlElement.h"
#	include "XmlElementListener.h"
#	include "XmlElementValueListener.h"

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
	XmlElementListener * parser_listener = parser->topListener();

	XmlElement element( parser, name, atts );

	parser_listener->parseXML( &element );

	XmlElementListener * incref_listener = parser->topListener();
	incref_listener->incref();
}
//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbEndElement( void *userData, const char *name )
{
	XmlExpatParser * parser = static_cast<XmlExpatParser*>( userData );
	XmlElementListener * parser_listener = parser->topListener();
	if( parser_listener->decref() )
	{
		parser_listener->parseXML( 0 );
		parser->popListener();
	}
}
//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbCharacterDataHandler( void *userData, const XML_Char *s, int len)
{
	XmlExpatParser * parser = static_cast<XmlExpatParser*>( userData );
	parser->callValueListener( s, len );
}
//////////////////////////////////////////////////////////////////////////
XmlExpatParser::XmlExpatParser()
	: m_valueListener(0)
{
}
//////////////////////////////////////////////////////////////////////////
XmlExpatParser::~XmlExpatParser()
{
	
}
//////////////////////////////////////////////////////////////////////////
void * XmlExpatParser::makeParser()
{
	XML_Parser parser = XML_ParserCreate(NULL);
	return parser;
}
//////////////////////////////////////////////////////////////////////////
void * XmlExpatParser::makeBuffer( void * _parser, size_t _size )
{
	return XML_GetBuffer( (XML_Parser)_parser, _size );
}
//////////////////////////////////////////////////////////////////////////
bool XmlExpatParser::parseXML( void * _parser, size_t _size, XmlElementListener * _listener )
{
	XML_SetUserData( (XML_Parser)_parser, this );
	XML_SetElementHandler( (XML_Parser)_parser, cbBeginElement, cbEndElement );
	XML_SetCharacterDataHandler( (XML_Parser)_parser, cbCharacterDataHandler );

	pushListener( _listener );

	bool done = true;

	XML_Status status = XML_ParseBuffer( (XML_Parser)_parser, _size, XML_TRUE );

	if( status == XML_STATUS_ERROR )
	{
		fprintf(stderr,
			"%s at line %" XML_FMT_INT_MOD "u\n",
			XML_ErrorString(XML_GetErrorCode((XML_Parser)_parser)),
			XML_GetCurrentLineNumber((XML_Parser)_parser)
			);

		done = false;
	}

	clearListener();
	XML_ParserFree( (XML_Parser)_parser );

	return done;
}
//////////////////////////////////////////////////////////////////////////
void XmlExpatParser::pushListener( XmlElementListener * _listener )
{
	m_listStackListener.push_back( _listener );
}
//////////////////////////////////////////////////////////////////////////
XmlElementListener * XmlExpatParser::topListener()
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
//////////////////////////////////////////////////////////////////////////
void XmlExpatParser::setValueListener( XmlElementValueListener * _listener )
{
	m_valueListener = _listener;
}
//////////////////////////////////////////////////////////////////////////
void XmlExpatParser::callValueListener( const char * _value, int _len )
{
	if( m_valueListener == 0 )
	{
		return;
	}

	int begin_trim = -1;
	char ch = 0;
	do
	{
		ch = _value[ ++begin_trim ];
	}while( ch == ' ' || ch == '\t' && begin_trim < _len );

	int end_trim = _len;
	do
	{
		ch = _value[ --end_trim ];
	}while( ch == ' ' || ch == '\t' && begin_trim < end_trim );

	std::string str_value( _value + begin_trim, end_trim - begin_trim + 1 );

	m_valueListener->call( str_value.c_str() );
	delete m_valueListener;
	m_valueListener = 0;
}