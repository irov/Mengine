#	pragma once

#	include "XmlElement.h"
#	include "XmlListener.h"

#	include <string>

class XmlElement;
class XmlExpatParser;

class XmlParser
{
public:
	XmlParser();
	~XmlParser();

public:
	void * makeBuffer( int _size );
	bool parseBuffer( int _size, XmlListener * _listener );

protected:
	XmlExpatParser * m_xmlExpat;
};

namespace XmlParserCheckMethod
{
	template<class F>
	struct check_method_param0
	{
		template<class T> struct check_method_param0_impl{ typedef void param0_type; };

		template<class T> struct remove_refrences{ typedef T type; };
		template<class T> struct remove_refrences<T &>{	typedef T type; };
		template<class T> struct remove_refrences<const T &>{ typedef T type; };

		template<class R, class C, class P0>
		struct check_method_param0_impl< R (C::*)( P0 ) >
		{
			typedef typename remove_refrences<P0>::type param0_type;
		};

		typedef check_method_param0_impl<F> result_impl;
		typedef typename result_impl::param0_type type;
	};

	template<class O, class F>
	static void check_member( O _o, F _f, const char * _value )
	{
		typedef typename check_method_param0<F>::type type;

		type var;
		XmlParserCast::attribute_value_cast( var, _value );
		(_o->*_f)(var);
	}
}

namespace XmlParserElement
{
	bool element_compare_title( XmlElement * _element, const char * _title );
	const char * element_get_title( XmlElement * _element );
	void element_begin_attributes( XmlElement * _element );
	bool element_valid_attributes( XmlElement * _element );
	void element_next_attributes( XmlElement * _element );
	bool element_compare_attribute_key( XmlElement * _element, const char * _key );
	const char * element_get_attribute_value( XmlElement * _element );
	void element_push_listener( XmlElement * _element, XmlListener * _listenerMethod );

	template<class C, class F>
	static void element_push_member_listener( XmlElement * _element, C * _class, F f )
	{
		element_push_member_listener_impl( _element, new XmlListenerMethod( _class, f ) );
	}
}

namespace XmlParserCast
{
	bool attribute_value_cast( bool & _var, const char * _value );
	bool attribute_value_cast( unsigned int & _var, const char * _value );
	bool attribute_value_cast( int & _var, const char * _value );
	bool attribute_value_cast( float & _var, const char * _value );
	bool attribute_value_cast( std::string & _var, const char * _value );
}


#	define XML_SWITCH_NODE( element )\
	for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; )\
	for( XmlElement * xmlengine_element = element; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	for( bool xmlengine_elif = false; xmlengine_elif == false; xmlengine_elif = true )

#	define XML_CASE_NODE( node )\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_elif == false && XmlParserElement::element_compare_title( xmlengine_element, node ) == true ;\
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

/*#	define XML_CASE_ATTRIBUTE_MEMBER( key, member )\
	for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; )\
	for( ; xmlengine_parse_once == true && XmlParserElement::element_compare_attribute_key( xmlengine_element, key ); )\
	for( const char * xmlengine_value = XmlParserElement::element_get_attribute_value( xmlengine_element ); xmlengine_parse_once == true; )\
	for( ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	XmlParserCheckMethod::check_member( this, member, xmlengine_value  )
*/

/*# define XML_CASE_ATTRIBUTE_MEMBER( key, member )\
 for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true && XmlParserElement::element_compare_attribute_key( xmlengine_element, key ); xmlengine_parse_once = false )\
XmlParserCheckMethod::check_member( this, member, xmlengine_value  )
*/

/*# define XML_CASE_ATTRIBUTE_MEMBER( key, member )\
 for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; )\
 for( ; xmlengine_parse_once == true && XmlParserElement::element_compare_attribute_key( xmlengine_element, key ); xmlengine_parse_once = false )\
 for( const char * xmlengine_value = XmlParserElement::element_get_attribute_value( xmlengine_element ); xmlengine_parse_once == true; )\
 for( ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
 XmlParserCheckMethod::check_member( this, member, xmlengine_value  )
*/
 # define XML_CASE_ATTRIBUTE_MEMBER( key, member )\
 for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
 for( ; xmlengine_parse_once == true && XmlParserElement::element_compare_attribute_key( xmlengine_element, key );  )\
 for( const char * xmlengine_value = XmlParserElement::element_get_attribute_value( xmlengine_element ); xmlengine_parse_once == true; )\
 for( ; xmlengine_parse_once == true; xmlengine_parse_once = false )\
 XmlParserCheckMethod::check_member( this, member, xmlengine_value  )

#	define XML_CASE_VALUE_NODE( node, key, var )\
	XML_CASE_NODE( node )\
	{\
		XML_FOR_EACH_ATTRIBUTES()\
		{\
			XML_CASE_ATTRIBUTE( key, var );\
		}\
	}

#	define XML_PUSH_CLASS_LISTENER( listener, method )\
	do{ XmlParserElement::element_push_listener( xmlengine_element, xmlListenerMethod( listener, method ) ); } while(false)
