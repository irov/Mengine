#	include "XmlEngine.h"
//#	include "FileEngine.h"

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
	void XmlEngine::initialize()
	{
		m_format.define( DEF_Animation, "Animation" );
		m_format.define( DEF_Layer2D, "Layer2D" );
		m_format.define( DEF_Sprite, "Sprite" );
		m_format.define( DEF_TextField, "TextField" );
		m_format.define( DEF_HotSpot, "HotSpot" );
		m_format.define( DEF_Point, "Point" );

		m_format.attribute( ATTR_Name, "Name" );
		m_format.attribute( ATTR_Type, "Type" );
		m_format.attribute( ATTR_Value, "Value" );
		m_format.attribute( ATTR_Position, "Position" );
		m_format.attribute( ATTR_Direction, "Direction" );

		m_format.tag( TAG_Scene, "Scene" );
		m_format.tag( TAG_Node, "Node" )( ATTR_Name, TYPE_String )( ATTR_Type, TYPE_Define );
		m_format.tag( TAG_Size, "Size" )( ATTR_Value, TYPE_Float2 );
		m_format.tag( TAG_Scale, "Scale" )( ATTR_Value, TYPE_Float2 );
		m_format.tag( TAG_Point, "Point" )( ATTR_Value, TYPE_Float2 );
		m_format.tag( TAG_Main, "Main" )( ATTR_Value, TYPE_Bool );
		m_format.tag( TAG_ImageMap, "ImageMap" )( ATTR_Name, TYPE_String );
		m_format.tag( TAG_Transformation, "Transformation" )( ATTR_Position, TYPE_Float2 )( ATTR_Direction, TYPE_Float2 )( ATTR_Value, TYPE_Mat3 );
		m_format.tag( TAG_RenderArea, "RenderArea" )( ATTR_Value, TYPE_Float4 );
		m_format.tag( TAG_Animation, "Animation" )( ATTR_Name, TYPE_String );
		m_format.tag( TAG_Looping, "Looping" )( ATTR_Value, TYPE_Bool );
		m_format.tag( TAG_AutoStart, "AutoStart" )( ATTR_Value, TYPE_Bool );
		m_format.tag( TAG_Font, "Font" )( ATTR_Name, TYPE_String );
		m_format.tag( TAG_Color, "Color" )( ATTR_Value, TYPE_Float4 );
		m_format.tag( TAG_CenterAlign, "CenterAlign" )( ATTR_Value, TYPE_Bool );
		m_format.tag( TAG_OutlineColor, "OutlineColor" )( ATTR_Value, TYPE_Float4 );
		m_format.tag( TAG_Outline, "Outline" )( ATTR_Value, TYPE_Bool );
		m_format.tag( TAG_CharOffset, "CharOffset" )( ATTR_Value, TYPE_Float );
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlFile( const String& _file, XmlElementListener * _listener )
	{
		if( Holder<FileEngine>::hostage()
			->existFile( _file ) == false )
		{
			return false;
		}

		DataStreamInterface * file = Holder<FileEngine>::hostage()
			->openFile( _file );

		if( file == 0 )
		{
			return false;
		}

		bool result = parseXmlFile( file, _listener );

		Holder<FileEngine>::hostage()
			->closeStream( file );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlFile( DataStreamInterface* _file, XmlElementListener * _listener )
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

		memcpy( buffer, _buffer.c_str(), size );

		bool result = XmlParser::parseBuffer( parser, size, _listener );

		if( new_parser )
		{
			XmlParser::deleteParser( parser );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlBuffer( const TVectorChar & _buffer, XmlElementListener * _listener )
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

		memcpy( buffer, &_buffer[0], size );

		bool result = XmlParser::parseBuffer( parser, size, _listener );

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
	bool attribute_value_cast( Menge::Resolution & _var, const Menge::TChar * _value )
	{
		int res = sscanf( _value, "%d;%d", &_var[0], &_var[1] );

		return res == 2;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec2f & _var, const char * _value )
	{
		int res = sscanf( _value, "%f;%f", &_var.x, &_var.y );

		return res == 2;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec3f & _var, const char * _value )
	{
		int res = sscanf( _value, "%f;%f;%f", &_var.x, &_var.y, &_var.z );

		return res == 3;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::quatf & _var, const char * _value )
	{
		int res = sscanf( _value, "%f;%f;%f;%f", &_var.x, &_var.y, &_var.z, &_var.w );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec4f & _var, const char * _value )
	{
		int res = sscanf( _value, "%f;%f;%f;%f", &_var.x, &_var.y, &_var.z, &_var.w );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( Menge::ColourValue & _var, const char * _value )
	{
		float a, r, g, b;
		int res = sscanf( _value, "%f;%f;%f;%f", &r, &g, &b, &a );

		_var.setA( a );
		_var.setR( r );
		_var.setG( g );
		_var.setB( b );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::mat3f & _var, const char * _value )
	{
		mt::ident_m3( _var );

		int res = sscanf( _value, 
			"%f;%f;%f;%f;%f;%f;"
			, &_var.v0.x, &_var.v0.y 
			, &_var.v1.x, &_var.v1.y 
			, &_var.v2.x, &_var.v2.y
			);

		return res == 6;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::mat4f & _var, const char * _value )
	{
		mt::ident_m4( _var );

		int res = sscanf( _value, 
			"%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;"
			, &_var.v0.x, &_var.v0.y, &_var.v0.z
			, &_var.v1.x, &_var.v1.y, &_var.v1.z
			, &_var.v2.x, &_var.v2.y, &_var.v2.z
			, &_var.v3.x, &_var.v3.y, &_var.v3.z
			);

		return res == 12;
	}
}