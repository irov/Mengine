#	pragma once

#	include "Holder.h"

#	include "XmlParser/XmlParser.h"
#	include "Math/mat4.h"
#	include "Math/quat.h"
#	include "FileEngine.h"
#	include "ColourValue.h"
#	include "Resolution.h"

#	include "XmlFormat.h"

#	include	"Config/Typedef.h"

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
		void initialize();

	public:
		bool parseXmlFile( const String& _file, XmlElementListener * _listener );
		bool parseXmlFile( DataStreamInterface* _file, XmlElementListener * _listener );
		bool parseXmlString( const String& _string, XmlElementListener * _listener );
		bool parseXmlBuffer( const TVectorChar & _buffer, XmlElementListener * _listener );

		template<class C, class F>
		bool parseXmlBufferM( const TVectorChar & _buffer, C * _self, F _method )
		{
			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
			bool result = parseXmlBuffer( _buffer, listener );
			return result;
		}

		template<class C, class F>
		bool parseXmlBufferM( const String& _buffer, C * _self, F _method )
		{
			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
			bool result = parseXmlString( _buffer, listener );
			return result;
		}
		

		template<class C, class F>
		bool parseXmlFileM( const String& _file, C * _self, F _method )
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
		bool parseXmlFileF( const String& _file, F f )
		{
			XmlElementListener * listener = new XmlElementListenerFunction<F>(f);
			return parseXmlFile( _file, listener );
		}

	protected:
		XmlExpatParser * m_parser;

		XmlFormat m_format;
	};
}

namespace XmlParserCast
{
	bool attribute_value_cast( Menge::Resolution & _var, const Menge::TChar * _value );
	bool attribute_value_cast( mt::vec2f & _var, const Menge::TChar * _value );
	bool attribute_value_cast( mt::vec3f & _var, const Menge::TChar * _value );
	bool attribute_value_cast( mt::vec4f & _var, const Menge::TChar * _value );
	bool attribute_value_cast( Menge::ColourValue & _var, const Menge::TChar * _value );
	bool attribute_value_cast( mt::quatf & _var, const Menge::TChar * _value );
	bool attribute_value_cast( mt::mat3f & _var, const Menge::TChar * _value );
	bool attribute_value_cast( mt::mat4f & _var, const Menge::TChar * _value );
}