#	pragma once

#	include "Config/Typedef.h"
//#	include <string>

namespace XmlParserCast
{
	bool attribute_value_cast( bool & _var, const Menge::TChar * _value );
	bool attribute_value_cast( int & _var, const Menge::TChar * _value );
	bool attribute_value_cast( unsigned int & _var, Menge::TChar * _value );
	bool attribute_value_cast( float & _var, const Menge::TChar * _value );
	bool attribute_value_cast( Menge::String & _var, const Menge::TChar * _value );
}