#	include "XmlEngine.h"

#	include "XmlTag.h"

#	include "Utils.h"

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
		//m_compiler.define( DEF_Animation, MENGE_TEXT("Animation") );
		//m_compiler.define( DEF_Layer2D, MENGE_TEXT("Layer2D") );
		//m_compiler.define( DEF_Sprite, MENGE_TEXT("Sprite") );
		//m_compiler.define( DEF_TextField, MENGE_TEXT("TextField") );
		//m_compiler.define( DEF_HotSpot, MENGE_TEXT("HotSpot") );
		//m_compiler.define( DEF_Point, MENGE_TEXT("Point") );

		//m_compiler.attribute( ATTR_Name, MENGE_TEXT("Name") );
		//m_compiler.attribute( ATTR_Type, MENGE_TEXT("Type") );
		//m_compiler.attribute( ATTR_Value, MENGE_TEXT("Value") );
		//m_compiler.attribute( ATTR_Position, MENGE_TEXT("Position") );
		//m_compiler.attribute( ATTR_Direction, MENGE_TEXT("Direction") );

		//XmlTag * tagScene = m_compiler.tag( TAG_Scene, MENGE_TEXT("Scene") );

		//XmlTag * tagNode = m_compiler.tag( TAG_Node, MENGE_TEXT("Node"), tagScene )->attribute( ATTR_Name, TYPE_String )->attribute( ATTR_Type, TYPE_Define );
		//{
		//	m_compiler.tag( TAG_Size, MENGE_TEXT("Size"), tagNode )->attribute( ATTR_Value, TYPE_Float2 );
		//	m_compiler.tag( TAG_Scale, MENGE_TEXT("Scale"), tagNode )->attribute( ATTR_Value, TYPE_Float2 );
		//	m_compiler.tag( TAG_Point, MENGE_TEXT("Point"), tagNode )->attribute( ATTR_Value, TYPE_Float2 );
		//	m_compiler.tag( TAG_Main, MENGE_TEXT("Main"), tagNode )->attribute( ATTR_Value, TYPE_Bool );
		//	m_compiler.tag( TAG_ImageMap, MENGE_TEXT("ImageMap"), tagNode )->attribute( ATTR_Name, TYPE_String );
		//	m_compiler.tag( TAG_Transformation, MENGE_TEXT("Transformation"), tagNode )->attribute( ATTR_Position, TYPE_Float2 )->attribute( ATTR_Direction, TYPE_Float2 )->attribute( ATTR_Value, TYPE_Mat3 );
		//	m_compiler.tag( TAG_RenderArea, MENGE_TEXT("RenderArea"), tagNode )->attribute( ATTR_Value, TYPE_Float4 );
		//	m_compiler.tag( TAG_Animation, MENGE_TEXT("Animation"), tagNode )->attribute( ATTR_Name, TYPE_String );
		//	m_compiler.tag( TAG_Looping, MENGE_TEXT("Looping"), tagNode )->attribute( ATTR_Value, TYPE_Bool );
		//	m_compiler.tag( TAG_AutoStart, MENGE_TEXT("AutoStart"), tagNode )->attribute( ATTR_Value, TYPE_Bool );
		//	m_compiler.tag( TAG_Font, MENGE_TEXT("Font"), tagNode )->attribute( ATTR_Name, TYPE_String );
		//	m_compiler.tag( TAG_Color, MENGE_TEXT("Color"), tagNode )->attribute( ATTR_Value, TYPE_Float4 );
		//	m_compiler.tag( TAG_CenterAlign, MENGE_TEXT("CenterAlign"), tagNode )->attribute( ATTR_Value, TYPE_Bool );
		//	m_compiler.tag( TAG_OutlineColor, MENGE_TEXT("OutlineColor"), tagNode )->attribute( ATTR_Value, TYPE_Float4 );
		//	m_compiler.tag( TAG_Outline, MENGE_TEXT("Outline"), tagNode )->attribute( ATTR_Value, TYPE_Bool );
		//	m_compiler.tag( TAG_CharOffset, MENGE_TEXT("CharOffset"), tagNode )->attribute( ATTR_Value, TYPE_Float );
		//}

		//XmlTag * tagDataBlock = m_compiler.tag( TAG_DataBlock, MENGE_TEXT("DataBlock"), tagNode )->attribute( ATTR_Name, TYPE_String )->attribute( ATTR_Type, TYPE_Define );
		//{
		//	m_compiler.tag( TAG_File, MENGE_TEXT("File"), tagDataBlock )->attribute( ATTR_Path, TYPE_String );

		//	XmlTag * tagSequences = m_compiler.tag( TAG_Sequences, MENGE_TEXT("Sequences"), tagDataBlock );
		//	{
		//		m_compiler.tag( TAG_SequenceArray, MENGE_TEXT("SequenceArray"), tagSequences );
		//	}
		//}

		//XmlTag * tagResources = m_compiler.tag( TAG_Resources, MENGE_TEXT("Resources"), tagNode );
		//{
		//	XmlTag * tagResourceScenes = m_compiler.tag( TAG_Scenes, MENGE_TEXT("Scenes"), tagResources );
		//	{
		//		m_compiler.tag( TAG_VALUE, MENGE_TEXT(""), tagResourceScenes );
		//	}			
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlFile( const String& _file, XmlElementListener * _listener )
	{
		if( Holder<FileEngine>::hostage()
			->existFile( _file ) == false )
		{
			return false;
		}

		//String::size_type it_last = _file.find_last_of( '.' );

		//String file_compile = _file.substr( 0, it_last );
		//file_compile += MENGE_TEXT(".xmlc");

		//m_compiler.compile( _file, file_compile );

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

		//memcpy( buffer, _buffer.c_str(), size );
		StringA str = Utils::WToA( _buffer );
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
	bool XmlEngine::parseXmlBuffer( const Blobject & _buffer, XmlElementListener * _listener )
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
		int res = STDSSCANF( _value, MENGE_TEXT("%d;%d"), &_var[0], &_var[1] );

		return res == 2;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec2f & _var, const Menge::TChar * _value )
	{
		int res = STDSSCANF( _value, MENGE_TEXT("%f;%f"), &_var.x, &_var.y );

		return res == 2;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec3f & _var, const Menge::TChar * _value )
	{
		int res = STDSSCANF( _value, MENGE_TEXT("%f;%f;%f"), &_var.x, &_var.y, &_var.z );

		return res == 3;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::quatf & _var, const Menge::TChar * _value )
	{
		int res = STDSSCANF( _value, MENGE_TEXT("%f;%f;%f;%f"), &_var.x, &_var.y, &_var.z, &_var.w );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::vec4f & _var, const Menge::TChar * _value )
	{
		int res = STDSSCANF( _value, MENGE_TEXT("%f;%f;%f;%f"), &_var.x, &_var.y, &_var.z, &_var.w );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( Menge::ColourValue & _var, const Menge::TChar * _value )
	{
		float a, r, g, b;
		int res = STDSSCANF( _value, MENGE_TEXT("%f;%f;%f;%f"), &r, &g, &b, &a );

		_var.setA( a );
		_var.setR( r );
		_var.setG( g );
		_var.setB( b );

		return res == 4;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::mat3f & _var, const Menge::TChar * _value )
	{
		mt::ident_m3( _var );

		int res = STDSSCANF( _value
			, MENGE_TEXT("%f;%f;%f;%f;%f;%f;")
			, &_var.v0.x, &_var.v0.y 
			, &_var.v1.x, &_var.v1.y 
			, &_var.v2.x, &_var.v2.y
			);

		return res == 6;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( mt::mat4f & _var, const Menge::TChar * _value )
	{
		mt::ident_m4( _var );

		int res = STDSSCANF( _value
			, MENGE_TEXT("%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;")
			, &_var.v0.x, &_var.v0.y, &_var.v0.z
			, &_var.v1.x, &_var.v1.y, &_var.v1.z
			, &_var.v2.x, &_var.v2.y, &_var.v2.z
			, &_var.v3.x, &_var.v3.y, &_var.v3.z
			);

		return res == 12;
	}
}
