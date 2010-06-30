#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class XmlElement;
	class ConstString;

	namespace XmlParserCast
	{
		bool attribute_value_cast( bool & _var, const Menge::TCharA * _value, XmlElement * _element );
		bool attribute_value_cast( int & _var, const Menge::TCharA * _value, XmlElement * _element );
		//bool attribute_value_cast( unsigned int & _var,const Menge::TChar * _value, XmlElement * _element );
		bool attribute_value_cast( std::size_t & _var,const Menge::TCharA * _value, XmlElement * _element );
		bool attribute_value_cast( float & _var, const Menge::TCharA * _value, XmlElement * _element );
		bool attribute_value_cast( Menge::String & _var, const Menge::TCharA * _value, XmlElement * _element );
		bool attribute_value_cast( Menge::ConstString & _var, const Menge::TCharA * _value, XmlElement * _element );
	}
}