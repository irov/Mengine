//#	pragma once
//
//#	include "Core/Holder.h"
//#	include "Math/mat4.h"
//#	include "Math/quat.h"
//#	include "FileEngine.h"
//#	include "Core/ColourValue.h"
//#	include "Core/Resolution.h"
//#	include "Core/Viewport.h"
//
//#	include "XmlParser.h"
//
//#	include	"Config/Typedef.h"
//
//
//
//namespace Menge
//{
//	class XmlElement;
//	class XmlExpatParser;
//
//	namespace XmlParserCast
//	{
//		bool attribute_value_cast( Menge::Viewport & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( Menge::Resolution & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( mt::vec2f & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( mt::vec3f & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( mt::vec4f & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( Menge::ColourValue & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( mt::quatf & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( mt::mat3f & _var, const Menge::TChar * _value, XmlElement * _element );
//		bool attribute_value_cast( mt::mat4f & _var, const Menge::TChar * _value, XmlElement * _element );
//	}
//
//	class XmlEngine
//		: public XmlParser
//		, public Holder<XmlEngine>
//	{
//	public:
//		XmlEngine();
//		~XmlEngine();
//
//	public:
//		bool parseXmlFile( const ConstString& _fileSystemName, const String& _file, XmlElementListener * _listener );
//		bool parseXmlFile( FileInputInterface* _file, XmlElementListener * _listener );
//		bool parseXmlString( const String& _string, XmlElementListener * _listener );
//		//bool parseXmlBuffer( const TBlobject & _buffer, XmlElementListener * _listener );
//
//		bool parseXmlBuffer( const void * _buffer, std::size_t _size, XmlElementListener * _listener );
//
//	public:
//		template<class C, class F>
//		bool parseXmlBufferM( const void * _buffer, std::size_t _size, C * _self, F _method )
//		{
//			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
//			bool result = parseXmlBuffer( _buffer, _size, listener );
//			return result;
//		}
//
//		template<class C, class F>
//		bool parseXmlBufferM( const TBlobject & _buffer, C * _self, F _method )
//		{
//			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
//			bool result = parseXmlBuffer( &_buffer[0], _buffer.size(), listener );
//			return result;
//		}
//
//		template<class C, class F>
//		bool parseXmlBufferM( const String& _buffer, C * _self, F _method )
//		{
//			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
//			bool result = parseXmlString( _buffer, listener );
//			return result;
//		}
//		
//		template<class C, class F>
//		bool parseXmlFileM( const ConstString& _fileSystemName, const String& _file, C * _self, F _method )
//		{
//			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
//			bool result = parseXmlFile( _fileSystemName, _file, listener );
//			return result;
//		}
//
//		template<class C, class F>
//		bool parseXmlFileM( FileInput* _file, C * _self, F _method )
//		{
//			XmlElementListener * listener = new XmlElementListenerMethod<C,F>(_self, _method );
//			bool result = parseXmlFile( _file, listener );
//			return result;
//		}
//
//		template<class F>
//		bool parseXmlFileF( const ConstString& _file, F f )
//		{
//			XmlElementListener * listener = new XmlElementListenerFunction<F>(f);
//			return parseXmlFile( _file, listener );
//		}
//
//	protected:
//		XmlExpatParser * m_parser;
//	};
//}
//
