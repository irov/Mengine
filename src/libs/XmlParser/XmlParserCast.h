#	pragma once

#	include <string>

namespace XmlParserCast
{
	bool attribute_value_cast( bool & _var, const char * _value );
	bool attribute_value_cast( unsigned int & _var, const char * _value );
	bool attribute_value_cast( int & _var, const char * _value );
	bool attribute_value_cast( float & _var, const char * _value );
	bool attribute_value_cast( std::string & _var, const char * _value );
}