#	include "Parser.h"

#include <stdio.h>
#include "expat.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

namespace XMLParser
{
	//////////////////////////////////////////////////////////////////////////
	static void XMLCALL cbBeginElement( void *userData, const char *name, const char **atts )
	{
		Parser * parser = static_cast<Parser*>( userData );
		Listener * listener = parser->topListener();

		Element element( parser, name, atts );

		listener->loader( &element );
	}
	//////////////////////////////////////////////////////////////////////////
	static void XMLCALL cbEndElement( void *userData, const char *name )
	{
		Parser * parser = static_cast<Parser*>( userData );
		parser->popListener();
	}
	//////////////////////////////////////////////////////////////////////////
	Parser::Parser()
	{
		m_parser = XML_ParserCreate(NULL);

		XML_SetUserData( m_parser, this );
		XML_SetElementHandler( m_parser, cbBeginElement, cbEndElement );
	}
	//////////////////////////////////////////////////////////////////////////
	Parser::~Parser()
	{
		XML_ParserFree( m_parser );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Parser::parseXML( const char * _source, size_t _size, Listener * _listener )
	{
		pushListener( _listener );

		XML_Status status = XML_Parse( m_parser, _source, _size-1, 1);
		
		popListener();

		if( status == XML_STATUS_ERROR ) 
		{
			fprintf(stderr,
				"%s at line %" XML_FMT_INT_MOD "u\n",
				XML_ErrorString(XML_GetErrorCode(m_parser)),
				XML_GetCurrentLineNumber(m_parser)
				);

			return false;
		}

		return true;
	}

	void Parser::pushListener( Listener * _listener )
	{
		m_listStackListener.push_back( _listener );
	}

	Listener * Parser::topListener()
	{
		return m_listStackListener.back();
	}

	void Parser::popListener()
	{
		m_listStackListener.pop_back();
	}
}