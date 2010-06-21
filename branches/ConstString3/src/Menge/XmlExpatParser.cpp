#	include "XmlExpatParser.h"
#	include <stdio.h>

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

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void XMLCALL cbBeginElement( void *userData, const Menge::TCharA *name, const Menge::TCharA **atts )
	{
		XmlExpatParser * parser = static_cast<XmlExpatParser*>( userData );
		XmlElementListener * parser_listener = parser->topListener();

		XmlElement element( parser, name, atts );
		parser_listener->parseXML( &element );

		XmlElementListener * incref_listener = parser->topListener();
		incref_listener->incref();
	}
	//////////////////////////////////////////////////////////////////////////
	static void XMLCALL cbEndElement( void *userData, const Menge::TCharA *name )
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
		m_parser = XML_ParserCreate(NULL);
		XML_SetUnknownEncodingHandler( m_parser, &XmlExpatParser::s_unknownEncodingHandler, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	XmlExpatParser::~XmlExpatParser()
	{
		XML_ParserFree( m_parser );
	}
	//////////////////////////////////////////////////////////////////////////
	void * XmlExpatParser::makeBuffer( std::size_t _size )
	{
		return XML_GetBuffer( m_parser, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlExpatParser::parseXML( std::size_t _size, XmlElementListener * _listener )
	{
		XML_SetUserData( m_parser, this );
		XML_SetElementHandler( m_parser, cbBeginElement, cbEndElement );
		pushListener( _listener );

		bool done = true;

		XML_Status status = XML_ParseBuffer( m_parser, _size, XML_TRUE );

		if( status == XML_STATUS_ERROR )
		{
			fprintf(stderr,
				"%s at line %" XML_FMT_INT_MOD "\n",
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
	bool XmlExpatParser::parseStatus()
	{
		XML_ParsingStatus status;
		XML_GetParsingStatus( m_parser, &status );

		return status.parsing == XML_PARSING;
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

		XML_SetCharacterDataHandler( m_parser, cbCharacterDataHandler );
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlExpatParser::callValueListener( const Menge::TCharA * _value, int _len )
	{
		if( m_valueListener == 0 )
		{
			return;
		}

		int begin_trim = -1;
		Menge::TChar ch = 0;
		do
		{
			ch = _value[ ++begin_trim ];
		}while( ch == ' ' || ch == '\t' && begin_trim < _len );

		int end_trim = _len;
		do
		{
			ch = _value[ --end_trim ];
		}while( ch == ' ' || ch == '\t' && begin_trim < end_trim );

		Menge::String str_value( _value + begin_trim, end_trim - begin_trim + 1 );

		m_valueListener->call( str_value.c_str() );
		XML_SetCharacterDataHandler( m_parser, 0 );
		delete m_valueListener;
		m_valueListener = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	int XmlExpatParser::s_unknownEncodingHandler(void *encodingHandlerData, const XML_Char *name, XML_Encoding *info)
	{
		int codeMap[128] = 
		{ 0x402, 0x403, 0x201A, 0x453, 0x201E, 0x2026, 0x2020, 0x2021, 0x20AC, 0x2030, 0x409, 0x2039, 0x40A, 0x40C, 0x40B, 0x40F,
		0x452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0, 0x2122, 0x459, 0x203A, 0x45A, 0x45C, 0x45B, 0x45F,
		0xA0, 0x40E, 0x45E, 0x408, 0xA4, 0x490, 0xA6, 0xA7, 0x401, 0xA9, 0x404, 0xAB, 0xAC, 0xAD, 0xAE, 0x407,
		0xB0, 0xB1, 0x406, 0x456, 0x491, 0xB5, 0xB6, 0xB7, 0x451, 0x2116, 0x454, 0xBB, 0x458, 0x405, 0x455, 0x457,
		0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416, 0x417, 0x418, 0x419, 0x41A, 0x41B, 0x41C, 0x41D, 0x41E, 0x41F,
		0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427, 0x428, 0x429, 0x42A, 0x42B, 0x42C, 0x42D, 0x42E, 0x42F,
		0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D, 0x43E, 0x43F,
		0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44A, 0x44B, 0x44C, 0x44D, 0x44E, 0x44F };

		for( int i = 0; i < 128; i++ )
		{
			info->map[i] = i;
		}
		for( int i = 128; i < 256; i++ )
		{
			info->map[i] = codeMap[i-128];
		}
		return XML_STATUS_OK;
	}
}