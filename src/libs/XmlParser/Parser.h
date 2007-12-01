#	pragma once

#	include "Element.h"
#	include "Listener.h"

#	include <list>

struct XML_ParserStruct;

namespace XMLParser
{
	class Listener;

	class Parser
	{
	public:
		Parser();
		~Parser();

	public:
		bool parseXML( const char * _source, size_t _size, Listener * _listener );

	public:
		void pushListener( Listener * _listener );
		Listener * topListener();
		void popListener();

	protected:
		typedef std::list<Listener *> TListStackListener;
		TListStackListener m_listStackListener;

		XML_ParserStruct * m_parser;
	};
}

#	define XML_CHECK_ELEMENT( parser )\
	for( bool xmlirov_parse_once = true ; xmlirov_parse_once == true; )\
	for( XMLParser::Element * xmlirov_parser = parser; xmlirov_parse_once == true; )\
	for( ; xmlirov_parse_once == true; xmlirov_parse_once = false)\
	for( bool xmlirov_elif = false; xmlirov_elif == false;)

#	define XML_CHECK_NODE( node )\
	if( xmlirov_elif == true ) continue; else\
	if( xmlirov_parser->compare( node ) )\
	for( ;xmlirov_elif == false; xmlirov_elif = true )

#	define XML_PUSH_LISTENER( listener )\
	xmlirov_parser->pushListener( listener )