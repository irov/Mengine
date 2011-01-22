#	include "gibson2movie.h"

#	include "tinyxml/tinyxml.h"

#	include <sstream>
#	include <fstream>

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

	static bool s_writeBool( std::ofstream & _stream, const char * _str )
	{
		int valueInt;
		if( sscanf_s( _str, "%d", &valueInt ) != 1)
		{
			return false;
		}

		bool value = (valueInt == 1);

		s_writeStream( _stream, value );

		return true;
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

	static bool s_writeSizet( std::ofstream & _stream, const char * _str )
	{
		size_t value;
		if( sscanf_s( _str, "%u", &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeSizet2( std::ofstream & _stream, const char * _str )
	{
		size_t value[2];
		if( sscanf_s( _str, "%d;%d", &value[0], &value[1] ) != 2)
		{
			return false;
		}

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeUnsignedShort( std::ofstream & _stream, const char * _str )
	{
		unsigned int valueInt;
		if( sscanf_s( _str, "%u", &valueInt ) != 1)
		{
			return false;
		}

		unsigned short value = (unsigned short)valueInt;

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeFloat( std::ofstream & _stream, const char * _str )
	{
		float value;
		if( sscanf_s( _str, "%f", &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeFloat2( std::ofstream & _stream, const char * _str )
	{
		float value[2];
		if( sscanf_s( _str, "%f,%f", &value[0], &value[1] ) != 2)
		{
			return false;
		}

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeFloat2_3( std::ofstream & _stream, const char * _str )
	{
		float value[3];
		if( sscanf_s( _str, "%f,%f,%f", &value[0], &value[1], &value[2] ) != 2)
		{
			return false;
		}

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeFloat4( std::ofstream & _stream, const char * _str )
	{
		float value[4];
		if( sscanf_s( _str, "%f;%f;%f;%f", &value[0], &value[1], &value[2], &value[3] ) != 4)
		{
			return false;
		}

		s_writeStream( _stream, value, 4 );

		return true;
	}

	static bool s_writeFloat6( std::ofstream & _stream, const char * _str )
	{
		float value[6];
		if( sscanf_s( _str 
			, "%f;%f;%f;%f;%f;%f"
			, &value[0], &value[1]
		, &value[2], &value[3] 
		, &value[4], &value[5] ) != 6 )
		{
			return false;
		}

		s_writeStream( _stream, value, 6 );

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
	std::size_t countElement( TiXmlNode * _node )
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

	for( TiXmlNode * movie = doc.FirstChild(); movie; movie = doc.IterateChildren( movie ) )
	{
		TiXmlElement * movie_element = movie->ToElement();

		if( movie_element == 0 )
		{
			continue;
		}

		const char * movie_service = movie_element->Attribute("service");

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


			std::size_t composition_size = Helper::countElement( composition );
			s_writeStream( fs, composition_size );

			for( TiXmlNode * layer = composition->FirstChild(); layer; layer = composition->IterateChildren( layer ) )
			{
				TiXmlElement * layer_element = layer->ToElement();

				if( layer_element == 0 )
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

				s_writeFloat( fs, layer_in );
				s_writeFloat( fs, layer_out );

				std::size_t group_size = Helper::countElement( layer );
				s_writeStream( fs, group_size );

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

						//ToDo
						s_writeStream( fs, 0 );
						s_writeString( fs, source_path );
					}
					else if( strcmp( group_value, "group" ) == 0 )
					{
						const char * group_name = group_element->Attribute( "name" );

						if( strcmp( group_name, "Transform" ) == 0 )
						{
							s_writeStream( fs, 1 );

							for( TiXmlNode * property = group->FirstChild(); property; property = group->IterateChildren( property ) )
							{
								TiXmlElement * property_element = property->ToElement();

								if( property_element == 0 )
								{
									continue;
								}

								const char * property_type = property_element->Attribute( "type" );

								if( strcmp( property_type, "Anchor_Point" ) == 0 )
								{
									s_writeStream( fs, 0 );

									for( TiXmlNode * key = property->FirstChild(); key; key = property->IterateChildren( key ) )
									{
										TiXmlElement * key_element = key->ToElement();

										if( key_element == 0 )
										{
											continue;
										}

										const char * key_value = key_element->Attribute( "value" );

										//ToDo
										s_writeFloat2_3( fs, key_value );
									}
								}
								else if( strcmp( property_type, "Position" ) == 0 )
								{
									s_writeStream( fs, 1 );

									for( TiXmlNode * key = property->FirstChild(); key; key = property->IterateChildren( key ) )
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
										s_writeFloat2_3( fs, key_value );
										s_writeFloat( fs, key_time );

										if( key_interp_in != 0 )
										{
											if( strcmp( key_interp_in, "Linear" ) == 0 )
											{
												s_writeStream( fs, 1 );
											}
										}
										else
										{
											s_writeStream( fs, 0 );
										}
									}
								}
								else if( strcmp( property_type, "Scale" ) == 0 )
								{
									s_writeStream( fs, 2 );

									for( TiXmlNode * key = property->FirstChild(); key; key = property->IterateChildren( key ) )
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
										s_writeFloat2_3( fs, key_value );
										s_writeFloat( fs, key_time );

										if( key_interp_in != 0 )
										{
											if( strcmp( key_interp_in, "Linear" ) == 0 )
											{
												s_writeStream( fs, 1 );
											}
										}
										else
										{
											s_writeStream( fs, 0 );
										}
									}
								}
								else if( strcmp( property_type, "Rotation" ) == 0 )
								{
									for( TiXmlNode * key = property->FirstChild(); key; key = property->IterateChildren( key ) )
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
										s_writeFloat( fs, key_value );
										s_writeFloat( fs, key_time );

										if( key_interp_in != 0 )
										{
											if( strcmp( key_interp_in, "Linear" ) == 0 )
											{
												s_writeStream( fs, 1 );
											}
										}
										else
										{
											s_writeStream( fs, 0 );
										}
									}
								}
								else if( strcmp( property_type, "Opacity" ) == 0 )
								{
									for( TiXmlNode * key = property->FirstChild(); key; key = property->IterateChildren( key ) )
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
										s_writeFloat( fs, key_value );
										s_writeFloat( fs, key_time );

										if( key_interp_in != 0 )
										{
											if( strcmp( key_interp_in, "Linear" ) == 0 )
											{
												s_writeStream( fs, 1 );
											}
										}
										else
										{
											s_writeStream( fs, 0 );
										}
									}
								}
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