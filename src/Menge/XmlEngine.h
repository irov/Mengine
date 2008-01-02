#	pragma once

#	include "Holder.h"

#	include "XmlParser/XmlParser.h"
#	include "Math/mat4.h"

namespace Menge
{
	class XmlEngine
		: public XmlParser
	{
	public:
		bool parseXmlFile( const std::string & _file, XmlListener * _listener );
		bool parseXmlBuffer( const std::string & _buffer, XmlListener * _listener );

		template<class C, class F>
		bool parseXmlFileM( const std::string & _file, C * _self, F f )
		{
			XmlListener * listener = new XmlListenerMethod<C,F>(_self,f);
			return parseXmlFile( _file, listener );
		}
	};
}

namespace XmlParserCast
{
	bool attribute_value_cast( mt::vec2f & _var, const char * _value );
	bool attribute_value_cast( mt::vec3f & _var, const char * _value );
	bool attribute_value_cast( mt::vec4f & _var, const char * _value );
	bool attribute_value_cast( mt::mat3f & _var, const char * _value );
	bool attribute_value_cast( mt::mat4f & _var, const char * _value );
}