#	include "XmlEngine.h"
#	include "XmlElement.h"

#	include <cstdio>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlEngine::XmlEngine()
	{
		m_parser = XmlParser::newParser( );
	}
	//////////////////////////////////////////////////////////////////////////
	XmlEngine::~XmlEngine()
	{
		XmlParser::deleteParser( m_parser );
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlFile( const ConstString& _fileSystemName, const String& _file, XmlElementListener * _listener )
	{
		FileInputInterface* file = FileEngine::hostage()
			->openInputFile( _fileSystemName, _file );

		if( file == 0 )
		{
			return false;
		}

		bool result = parseXmlFile( file, _listener );

		FileEngine::hostage()
			->closeFileInput( file );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlFile( FileInputInterface* _file, XmlElementListener * _listener )
	{
		if( _file == 0 )
		{
			return false;				 
		}

		std::streamsize size = _file->size();

		XmlExpatParser * parser = m_parser;
		bool new_parser = false;

		if( XmlParser::parseStatus( m_parser ) == true )
		{
			parser = XmlParser::newParser();
			new_parser = true;
		}

		void * buffer = XmlParser::makeBuffer( parser, size );
		_file->read( buffer, size );
		_file->seek( 0 );
		
		bool result = XmlParser::parseBuffer( parser, size, _listener );

		if( new_parser )
		{
			XmlParser::deleteParser( parser );
		}

		return result;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlString( const String& _buffer, XmlElementListener * _listener )
	{
		std::string::size_type size = _buffer.size();

		XmlExpatParser * parser = m_parser;
		bool new_parser = false;

		if( XmlParser::parseStatus( m_parser ) == true )
		{
			parser = XmlParser::newParser();
			new_parser = true;
		}

		void * buffer = XmlParser::makeBuffer( parser, size );

		//memcpy( buffer, _buffer.c_str(), size );
		String str = _buffer;
		const char* chBuf = str.c_str();
		std::copy( chBuf, chBuf + size, (char*)buffer );

		bool result = XmlParser::parseBuffer( parser, size, _listener );

		if( new_parser )
		{
			XmlParser::deleteParser( parser );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlBuffer( const void * _buffer, std::size_t _size, XmlElementListener * _listener )
	{
		XmlExpatParser * parser = m_parser;
		bool new_parser = false;

		if( XmlParser::parseStatus( m_parser ) == true )
		{
			parser = XmlParser::newParser();
			new_parser = true;
		}

		void * buffer = XmlParser::makeBuffer( parser, _size );

		memcpy( buffer, _buffer, _size );

		bool result = XmlParser::parseBuffer( parser, _size, _listener );

		if( new_parser )
		{
			XmlParser::deleteParser( parser );
		}

		return result;
	}
}

namespace XmlParserCast
{
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( Menge::Viewport & _var, const Menge::TChar * _value, XmlElement * _element )
	{
		int res = std::sscanf( _value, "%f;%f;%f;%f", &_var.begin.x, &_var.begin.y, &_var.end.x, &_var.end.y );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( Menge::Resolution & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		int res = std::sscanf( _value, "%d;%d", &_var[0], &_var[1] );

		return res == 2;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec2f & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		int res = std::sscanf( _value, "%f;%f", &_var.x, &_var.y );

		return res == 2;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec3f & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		int res = std::sscanf( _value, "%f;%f;%f", &_var.x, &_var.y, &_var.z );

		return res == 3;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::quatf & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		int res = std::sscanf( _value, "%f;%f;%f;%f", &_var.x, &_var.y, &_var.z, &_var.w );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec4f & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		int res = std::sscanf( _value, "%f;%f;%f;%f", &_var.x, &_var.y, &_var.z, &_var.w );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( Menge::ColourValue & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		float a, r, g, b;
		int res = std::sscanf( _value, "%f;%f;%f;%f", &r, &g, &b, &a );

		_var.setA( a );
		_var.setR( r );
		_var.setG( g );
		_var.setB( b );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::mat3f & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		mt::ident_m3( _var );

		int res = std::sscanf( _value
			, "%f;%f;%f;%f;%f;%f;"
			, &_var.v0.x, &_var.v0.y 
			, &_var.v1.x, &_var.v1.y 
			, &_var.v2.x, &_var.v2.y
			);

		return res == 6;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::mat4f & _var, const Menge::TCharA * _value, XmlElement * _element )
	{
		mt::ident_m4( _var );

		int res = std::sscanf( _value
			, "%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;"
			, &_var.v0.x, &_var.v0.y, &_var.v0.z
			, &_var.v1.x, &_var.v1.y, &_var.v1.z
			, &_var.v2.x, &_var.v2.y, &_var.v2.z
			, &_var.v3.x, &_var.v3.y, &_var.v3.z
			);

		return res == 12;
	}
}
