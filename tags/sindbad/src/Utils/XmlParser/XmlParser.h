#	pragma once

#	include "XmlElement.h"
#	include "XmlElementListener.h"
#	include "XmlElementValueListener.h"

#	include "XmlParserCheckMethod.h"
#	include "XmlParserCast.h"
#	include "XmlParserElement.h"

#	include <string>

class XmlElement;
class XmlExpatParser;

class XmlParser
{
public:
	XmlParser();
	~XmlParser();

public:
	XmlExpatParser * newParser();
	void deleteParser( XmlExpatParser * _parser );
	void * makeBuffer( XmlExpatParser * _parser, std::size_t _size );
	bool parseBuffer( XmlExpatParser * _parser, std::size_t _size, XmlElementListener * _listener );

	bool parseStatus( XmlExpatParser * _parser );

	template<class C, class F>
	bool parseBufferMember( std::size_t _size, C * _self, F _method )
	{
		XmlElementListener * listener = makeXmlElementListener( _self, _method );
		return parseBuffer( _size, listener );
	}

public:
	template<class C, class F>
	bool parseMember( const char * _xml, C * _self, F _method )
	{
		unsigned int size = strlen( _xml );
		void * buffer = makeBuffer( size );

		memcpy( buffer, _xml, size );
		
		XmlElementListener * listener = makeXmlElementListener( _self, _method );
		return parseBuffer( size, listener );
	}
};

#	define XML_SWITCH_NODE( element )\
	for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; )\
	for( XmlElement * xmlengine_element = element; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	for( bool xmlengine_elif = false; xmlengine_elif == false; xmlengine_elif = true )\
	if( xmlengine_element != 0 )

#	define XML_END_NODE()\
	else

#	define XML_CASE_NODE( node )\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_elif == false && XmlParserElement::element_compare_title( xmlengine_element, node ) == true ;\
	xmlengine_elif = true )

#	define XML_CASE_DEFAULT()\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_elif == false ;\
	xmlengine_elif = true )

#	define XML_TITLE_NODE\
	XmlParserElement::element_get_title( xmlengine_element )


#	define XML_FOR_EACH_ATTRIBUTES()\
	for( XmlParserElement::element_begin_attributes( xmlengine_element );\
	XmlParserElement::element_valid_attributes( xmlengine_element ) == true;\
	XmlParserElement::element_next_attributes( xmlengine_element ) )

#	define XML_CASE_ATTRIBUTE( key, var )\
	for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true && XmlParserElement::element_compare_attribute_key( xmlengine_element, key ); xmlengine_parse_once = false )\
	XmlParserCast::attribute_value_cast( var, XmlParserElement::element_get_attribute_value( xmlengine_element ) )

#	define XML_CASE_ATTRIBUTE_MEMBER( key, member )\
	for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	for( ; xmlengine_parse_once == true && XmlParserElement::element_compare_attribute_key( xmlengine_element, key );  )\
	for( const Menge::TChar * xmlengine_value = XmlParserElement::element_get_attribute_value( xmlengine_element ); xmlengine_parse_once == true; )\
	for( ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	XmlParserCheckMethod::check_member( this, member, xmlengine_value  )

#	define XML_CASE_ATTRIBUTE_MEMBERT( key, member, type )\
	for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	for( ; xmlengine_parse_once == true && XmlParserElement::element_compare_attribute_key( xmlengine_element, key );  )\
	for( const Menge::TChar * xmlengine_value = XmlParserElement::element_get_attribute_value( xmlengine_element ); xmlengine_parse_once == true; )\
	for( ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	XmlParserCheckMethod::check_member_t( this, member, xmlengine_value, XmlParserCheckMethod::type_wrap<type>() )

#	define XML_CASE_ATTRIBUTE_NODE( node, key, var )\
	XML_CASE_NODE( node )\
	{\
		XML_FOR_EACH_ATTRIBUTES()\
		{\
			XML_CASE_ATTRIBUTE( key, var );\
		}\
	}

#	define XML_CASE_ATTRIBUTE_NODE_METHOD( node, key, method )\
	XML_CASE_NODE( node )\
	{\
		XML_FOR_EACH_ATTRIBUTES()\
		{\
			XML_CASE_ATTRIBUTE_MEMBER( key, method );\
		}\
	}

#	define XML_CASE_ATTRIBUTE_NODE_METHODT( node, key, method, type )\
	XML_CASE_NODE( node )\
	{\
		XML_FOR_EACH_ATTRIBUTES()\
		{\
			XML_CASE_ATTRIBUTE_MEMBERT( key, method, type );\
		}\
	}

#	define XML_PARSE_VALUE_NODE_METHOD( method )\
	do{ XmlParserElement::element_set_listener_value( xmlengine_element, this, method ); } while(false)

#	define XML_CASE_VALUE_NODE_METHOD( node, method )\
	XML_CASE_NODE( node )\
	{\
		XML_PARSE_VALUE_NODE_METHOD( method );\
	}

#	define XML_PARSE_ELEMENT( listener, method )\
	do{ XmlParserElement::element_push_listener( xmlengine_element, listener, method ); } while(false)

#	define XML_PARSE_ELEMENT_ARG1( listener, method, arg1 )\
	do{ XmlParserElement::element_push_listener_a( xmlengine_element, listener, method, arg1 ); } while(false)

