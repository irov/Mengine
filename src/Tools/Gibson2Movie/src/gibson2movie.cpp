#	include "gibson2movie.h"

#	include "tinyxml/tinyxml.h"

#	include <sstream>
#	include <fstream>


char id_source = 1;
char id_anchor_point = 2;
char id_position = 3;
char id_scale = 4;
char id_rotation = 5;
char id_opacity = 6;

//////////////////////////////////////////////////////////////////////////
namespace
{
	template<class T>
	static void s_writeStream( std::ofstream & _stream, T _t )
	{
		_stream.write( (const char *)&_t, sizeof(T) );
	}

	template<class T>
	static void s_writeStream( std::ofstream & _stream, T * _t, std::streamsize i )
	{
		_stream.write( (const char *)_t, sizeof(*_t)*i );
	}

	static bool s_writeInt( std::ofstream & _stream, const char * _str )
	{
		int value;
		if( sscanf_s( _str, "%d", &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	void scaleParam( float * _buff, int _size, float _coef )
	{
		for( int i = 0; i != _size; ++i )
		{
			_buff[i] *= _coef;
		}
	}

	static bool s_writeFloat( std::ofstream & _stream, const char * _str, float _coef = 1.f )
	{
		float value;
		if( sscanf_s( _str, "%f", &value ) != 1)
		{
			return false;
		}

		scaleParam( &value, 1, _coef );

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeFloat2_3( std::ofstream & _stream, const char * _str, float _coef = 1.f )
	{
		float value[3];
		if( sscanf_s( _str, "%f,%f,%f", &value[0], &value[1], &value[2] ) != 3)
		{
			return false;
		}

		scaleParam( value, 2, _coef );

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeString( std::ofstream & _stream, const char * _str )
	{
		std::streamsize size = (std::streamsize)strlen(_str);
		s_writeStream( _stream, size );
		s_writeStream( _stream, _str, size );

		return true;
	}
};

namespace Helper
{
	static std::size_t countElement( TiXmlNode * _node )
	{
		std::size_t size = 0;

		for( TiXmlNode * n = _node->FirstChild(); n; n = _node->IterateChildren( n ) )
		{
			TiXmlElement * element = n->ToElement();

			if( element == 0 )
			{
				continue;
			}

			++size;
		}

		return size;
	}

	typedef bool (*FWriteFloat)( std::ofstream & , const char *, float );

	static void writeDefaultFrame( std::ofstream & _fs, TiXmlNode * _property, FWriteFloat _write, float _coef )
	{
		TiXmlNode * key = _property->FirstChild();

		TiXmlElement * key_element = key->ToElement();

		if( key_element == 0 )
		{
			return;
		}

		const char * key_value = key_element->Attribute( "value" );

		_write( _fs, key_value, _coef );
	}
	
	static void writeKeyFrame( std::ofstream & _fs, TiXmlNode * _property, FWriteFloat _write, float _coef )
	{
		for( TiXmlNode * key = _property->FirstChild(); key; key = _property->IterateChildren( key ) )
		{
			TiXmlElement * key_element = key->ToElement();

			if( key_element == 0 )
			{
				continue;
			}

			const char * key_value = key_element->Attribute( "value" );
			const char * key_time = key_element->Attribute( "time" );
			const char * key_interp_in = key_element->Attribute( "interp_in" );

			//ToDo
			_write( _fs, key_value, _coef );

			s_writeFloat( _fs, key_time, 1000.f );
			
			if( strcmp( key_interp_in, "Linear" ) == 0 )
			{
				s_writeStream( _fs, 1 );
			}
			else
			{
				s_writeStream( _fs, 0 );
			}
		}
	}

	static void writeProperty( std::ofstream & _fs, const char * _name, char _id, TiXmlNode * _property, FWriteFloat _write, float _coef = 1.f )
	{
		TiXmlElement * property_element = _property->ToElement();

		if( property_element == 0 )
		{
			return;
		}

		const char * property_type = property_element->Attribute( "type" );

		if( strcmp( property_type, _name ) == 0 )
		{
			s_writeStream( _fs, _id );

			std::size_t key_size = Helper::countElement( _property );

			if( key_size == 0 )
			{
				printf("error invalid key size");
				return;
			}
			else if( key_size == 1 )
			{
				s_writeStream( _fs, 1 );
				Helper::writeDefaultFrame( _fs, _property, _write, _coef );
			}
			else
			{
				s_writeStream( _fs, key_size );
				Helper::writeKeyFrame( _fs, _property, _write, _coef );
			}
		}
	}
}

bool Gibson2Movie::convert( const std::string & _gibson, const std::string & _movie )
{
	TiXmlDocument doc;

	if( doc.LoadFile( _gibson.c_str() ) == false )
	{
		printf("Invalid gibson file '%s' - '%s'"
			, _gibson.c_str()
			, doc.ErrorDesc()
			);

		return false;
	}

	std::ofstream fs( _movie.c_str(), std::ios_base::binary );

	char magic_number = 42 + 1; 
	s_writeStream( fs, magic_number );

	std::size_t movie_size = Helper::countElement( &doc );

	if( movie_size != 1 )
	{
		return false;
	}
	
	for( TiXmlNode * movie = doc.FirstChild(); movie; movie = doc.IterateChildren( movie ) )
	{
		TiXmlElement * movie_element = movie->ToElement();

		if( movie_element == 0 )
		{
			continue;
		}

		const char * movie_service = movie_element->Attribute("service");

		std::size_t composition_size = Helper::countElement( movie );

		if( composition_size != 1 )
		{
			return false;
		}

		for( TiXmlNode * composition = movie->FirstChild(); composition; composition = movie->IterateChildren( composition ) )
		{
			TiXmlElement * composition_element = composition->ToElement();

			if( composition_element == 0 )
			{
				continue;
			}

			const char * composition_name = composition_element->Attribute("name");
			const char * composition_duration = composition_element->Attribute("duration");
			const char * composition_width = composition_element->Attribute("width");
			const char * composition_height = composition_element->Attribute("height");

			//ToDo

			s_writeString( fs, composition_name );
			s_writeFloat( fs, composition_duration );
			s_writeFloat( fs, composition_width );
			s_writeFloat( fs, composition_height );


			std::size_t layer_size = Helper::countElement( composition );
			s_writeStream( fs, layer_size );

			for( TiXmlNode * layer = composition->FirstChild(); layer; layer = composition->IterateChildren( layer ) )
			{
				TiXmlElement * layer_element = layer->ToElement();

				if( layer_element == 0 )
				{
					continue;
				}

				const char * layer_treeD = layer_element->Attribute("threeD");

				if( strcmp( layer_treeD, "true" ) == 0 )
				{
					continue;
				}

				const char * layer_index = layer_element->Attribute("index");
				const char * layer_name = layer_element->Attribute("name");
				const char * layer_width = layer_element->Attribute("width");
				const char * layer_height = layer_element->Attribute("height");

				const char * layer_in = layer_element->Attribute("in");
				const char * layer_out = layer_element->Attribute("out");

				s_writeInt( fs, layer_index );
				s_writeString( fs, layer_name );
				s_writeFloat( fs, layer_width );
				s_writeFloat( fs, layer_height );

				s_writeFloat( fs, layer_in, 1000.f );
				s_writeFloat( fs, layer_out, 1000.f );

				for( TiXmlNode * group = layer->FirstChild(); group; group = layer->IterateChildren( group ) )
				{
					TiXmlElement * group_element = group->ToElement();

					if( group_element == 0 )
					{
						continue;
					}

					const char * group_value = group_element->Value();

					if( strcmp( group_value, "source" ) == 0 )
					{
						const char * source_path = group_element->Attribute( "path" );

						std::string path( source_path );
						std::size_t pos = path.find_last_of( '/' );
						std::string name = path.substr( pos + 1 ); 

						//ToDo
						s_writeStream( fs, id_source );
						s_writeString( fs, name.c_str() );
					}
					else if( strcmp( group_value, "group" ) == 0 )
					{
						const char * group_name = group_element->Attribute( "name" );

						if( strcmp( group_name, "Transform" ) == 0 )
						{
							for( TiXmlNode * property = group->FirstChild(); property; property = group->IterateChildren( property ) )
							{
								Helper::writeProperty( fs, "Anchor_Point", id_anchor_point, property, &s_writeFloat2_3 );
								Helper::writeProperty( fs, "Position", id_position, property, &s_writeFloat2_3 );
								Helper::writeProperty( fs, "Scale", id_scale, property, &s_writeFloat2_3, 1.f/100.f );
								Helper::writeProperty( fs, "Rotation", id_rotation, property, &s_writeFloat, 1.f/180.f );
								Helper::writeProperty( fs, "Opacity", id_opacity, property, &s_writeFloat, 1.f/100.f );
							}
						}
					}
				}
			}
		}
	}

	return true;
}


void main()
{
	Gibson2Movie g2m;

	g2m.convert( "Boards.xml", "Boards.mov" );
}