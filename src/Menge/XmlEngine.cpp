#	include "XmlEngine.h"
#	include "FileEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlFile( const std::string & _file, XmlElementListener * _listener )
	{
		FileDataInterface * file = Holder<FileEngine>::hostage()
			->openFile( _file );

		if( file == 0 )
		{
			return false;				 
		}

		size_t size = file->size();

		void * buffer = XmlParser::makeBuffer( size );
		file->read( buffer, size );

		return XmlParser::parseBuffer( size, _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlEngine::parseXmlBuffer( const std::string & _buffer, XmlElementListener * _listener )
	{
		size_t size = _buffer.size();

		void * buffer = XmlParser::makeBuffer( size );

		memcpy( buffer, _buffer.c_str(), size );

		return XmlParser::parseBuffer( size, _listener );
	}
}

namespace XmlParserCast
{
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
	bool attribute_value_cast( mt::vec4f & _var, const char * _value )
	{
		int res = sscanf( _value, "%f;%f;%f;%f", &_var.x, &_var.y, &_var.z, &_var.w );

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