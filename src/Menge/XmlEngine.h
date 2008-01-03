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

		template<class C, class F, class FE>
		bool parseXmlFileM( const std::string & _file, C * _self, F _method, FE _end )
		{
			XmlListener * listener = new XmlListenerMethod<C,F>(_self, _method );
			bool result = parseXmlFile( _file, listener );
			if( _end ) 
			{
				(_self->*_end)( result );
			}
			return result;
		}

		template<class C, class F>
		bool parseXmlFileM( const std::string & _file, C * _self, F _method )
		{
			XmlListener * listener = new XmlListenerMethod<C,F>(_self, _method );
			bool result = parseXmlFile( _file, listener );
			return result;
		}

		template<class F>
		bool parseXmlFileF( const std::string & _file, F f )
		{
			XmlListener * listener = new XmlListenerFunction<F>(f);
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