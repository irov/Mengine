#	include "XmlParser.h"
#	include "XmlExpatParser.h"

//////////////////////////////////////////////////////////////////////////
XmlParser::XmlParser()
{
	m_xmlExpat = new XmlExpatParser;
}
//////////////////////////////////////////////////////////////////////////
XmlParser::~XmlParser()
{
	delete m_xmlExpat;
}
//////////////////////////////////////////////////////////////////////////
void * XmlParser::makeBuffer( int _size )
{
	return m_xmlExpat->makeBuffer( _size );
}
//////////////////////////////////////////////////////////////////////////
bool XmlParser::parseBuffer( int _size, XmlListener * _listener )
{
	return m_xmlExpat->parseXML( _size, _listener );
}
//////////////////////////////////////////////////////////////////////////
template<class T>
static bool attribute_value_cast_format( T & _var, const char * _value, const char * _format )
{
	int result = sscanf_s( _value, _format, &_var);

	return result == 1;
}
//////////////////////////////////////////////////////////////////////////
namespace XmlParserCast
{
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( bool & _var, const char * _value )
	{
		unsigned int wrapp_value;
		if( attribute_value_cast_format( wrapp_value, _value, "%u" ) == false )
		{
			return false;
		}

		_var = (wrapp_value != 0);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( unsigned int & _var, const char * _value )
	{
		return attribute_value_cast_format( _var, _value, "%u" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( int & _var, const char * _value )
	{
		return attribute_value_cast_format( _var, _value, "%d" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( float & _var, const char * _value )
	{
		return attribute_value_cast_format( _var, _value, "%f" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( std::string & _var, const char * _value )
	{
		_var.assign( _value );

		return true;
	}
}

namespace XmlParserElement
{
	//////////////////////////////////////////////////////////////////////////
	bool element_compare_title( XmlElement * _element, const char * _title )
	{
		return _element->compareTitle( _title );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * element_get_title( XmlElement * _element )
	{
		return _element->getTitle();
	}
	//////////////////////////////////////////////////////////////////////////
	void element_begin_attributes( XmlElement * _element )
	{
		_element->beginAttributes();
	}
	//////////////////////////////////////////////////////////////////////////
	bool element_valid_attributes( XmlElement * _element )
	{
		return _element->validAttributes();
	}
	//////////////////////////////////////////////////////////////////////////
	void element_next_attributes( XmlElement * _element )
	{
		_element->nextAttributes();
	}
	//////////////////////////////////////////////////////////////////////////
	bool element_compare_attribute_key( XmlElement * _element, const char * _key )
	{
		return _element->compareAttributeKey( _key );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * element_get_attribute_value( XmlElement * _element )
	{
		return _element->getAttributeValue();
	}
	//////////////////////////////////////////////////////////////////////////
	void element_push_listener( XmlElement * _element, XmlListener * _listenerMethod )
	{
		_element->pushListener( _listenerMethod );
	}
}