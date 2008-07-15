#	pragma once

#	include "Holder.h"

#	include "XmlParser/XmlParser.h"
#	include "Math/mat4.h"
#	include "Math/quat.h"
#	include "FileEngine.h"
#	include "ColourValue.h"

class XmlExpatParser;

namespace Menge
{
	class XmlEngine
		: public XmlParser
	{
	public:
		XmlEngine();
		~XmlEngine();

	public:
		bool parseXmlFile( const std::string & _file, XmlElementListener * _listener );
		bool parseXmlFile( DataStreamInterface* _file, XmlElementListener * _listener );
		bool parseXmlString( const std::string & _string, XmlElementListener * _listener );
		bool parseXmlBuffer( const TVectorChar & _buffer, XmlElementListener * _listener );

		template<class C, class F>
		bool parseXmlBufferM( const TVectorChar & _buffer, C * _self, F _method )
		{
			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
			bool result = parseXmlBuffer( _buffer, listener );
			return result;
		}

		template<class C, class F>
		bool parseXmlBufferM( const std::string & _buffer, C * _self, F _method )
		{
			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
			bool result = parseXmlString( _buffer, listener );
			return result;
		}
		

		template<class C, class F>
		bool parseXmlFileM( const std::string & _file, C * _self, F _method )
		{
			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
			bool result = parseXmlFile( _file, listener );
			return result;
		}

		template<class C, class F>
		bool parseXmlFileM( DataStreamInterface* _file, C * _self, F _method )
		{
			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
			bool result = parseXmlFile( _file, listener );
			return result;
		}

		template<class F>
		bool parseXmlFileF( const std::string & _file, F f )
		{
			XmlElementListener * listener = new XmlElementListenerFunction<F>(f);
			return parseXmlFile( _file, listener );
		}

	protected:
		XmlExpatParser * m_parser;
	};
}

namespace XmlParserCast
{
	bool attribute_value_cast( mt::vec2f & _var, const char * _value );
	bool attribute_value_cast( mt::vec3f & _var, const char * _value );
	bool attribute_value_cast( mt::vec4f & _var, const char * _value );
	bool attribute_value_cast( Menge::ColourValue & _var, const char * _value );
	bool attribute_value_cast( mt::quatf & _var, const char * _value );
	bool attribute_value_cast( mt::mat3f & _var, const char * _value );
	bool attribute_value_cast( mt::mat4f & _var, const char * _value );
}