#	include "format.h"

#	include "../../dependencies/expat/lib/expat.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif


enum
{
	XMLC_TYPE_STRING,
	XMLC_TYPE_BOOL,
	XMLC_TYPE_INT,
	XMLC_TYPE_FLOAT,
	XMLC_TYPE_FLOAT2,
	XMLC_TYPE_FLOAT4,
	XMLC_TYPE_MAT3,
	XMLC_TYPE_DEFINE
};

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbBeginElementFormat( void *userData, const char *name, const char **attr )
{
	XmlCompile * format = static_cast<XmlCompile*>(userData);

	format->beginElementFormat( name, attr );
}

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbEndElementFormat( void *userData, const char *name )
{
	XmlCompile * format = static_cast<XmlCompile*>(userData);

	format->endElementFormat( name );
}

void XmlCompile::beginElementFormat( const char * _name, const char ** _attr )
{
	if( m_parserLevel == 1 )
	{
		if( strcmp( _name, "Tag" ) == 0 )
		{
			m_parserState = 1;
		}
		else if( strcmp( _name, "Attribute" ) == 0 )
		{
			m_parserState = 2;
		}
		else if( strcmp( _name, "Define" ) == 0 )
		{
			m_parserState = 3;
		}
	}
	else if( m_parserLevel > 1 )
	{
		if( m_parserState == 1 )
		{
			Tag & tag = m_tags[_name];

			tag.id = ++m_tagIds;

			for (int i = 0; _attr[i]; i += 2) 
			{
				TMapAttributes::iterator it_attribute_find = m_attributes.find( _attr[i] );

				if( it_attribute_find == m_attributes.end() )
				{
					printf("Xml Compiler Format load is failed [attribute '%s' not id\n"
						, _attr[i]
					);
					return;
				}

				char attrTypeId = it_attribute_find->second;

				char & attrId = tag.m_attributeTypes[ attrTypeId ];

				if( strcmp( _attr[i + 1], "String" ) == 0 )
				{
					attrId = XMLC_TYPE_STRING;
				}
				else if( strcmp( _attr[i + 1], "Bool" ) == 0 )
				{
					attrId = XMLC_TYPE_BOOL;
				}
				else if( strcmp( _attr[i + 1], "Int" ) == 0 )
				{
					attrId = XMLC_TYPE_INT;
				}
				else if( strcmp( _attr[i + 1], "Float" ) == 0 )
				{
					attrId = XMLC_TYPE_FLOAT;
				}
				else if( strcmp( _attr[i + 1], "Float2" ) == 0 )
				{
					attrId = XMLC_TYPE_FLOAT2;
				}
				else if( strcmp( _attr[i + 1], "Float4" ) == 0 )
				{
					attrId = XMLC_TYPE_FLOAT4;
				}
				else if( strcmp( _attr[i + 1], "Mat3" ) == 0 )
				{
					attrId = XMLC_TYPE_MAT3;
				}
				else if( strcmp( _attr[i + 1], "Define" ) == 0 )
				{
					attrId = XMLC_TYPE_DEFINE;
				}
			}
		}
		else if( m_parserState == 2 )
		{
			char & attribute = m_attributes[ _name ];
			attribute = ++m_attributeIds;
		}
		else if( m_parserState == 3 )
		{
			char & define = m_defines[ _name ];
			define = ++m_defineIds;
		}
	}

	++m_parserLevel;
}

void XmlCompile::endElementFormat( const char *name )
{
	--m_parserLevel;
}

void XmlCompile::load( const std::string & _fileFormat )
{
	FILE * file = fopen( _fileFormat.c_str(), "rb" );

	fseek( file, 0, SEEK_END );
	long size = ftell( file );

	XML_Parser parser = XML_ParserCreate(NULL);

	void * body = XML_GetBuffer( parser, size );

	fseek( file, 0, SEEK_SET );
	fread( body, 1, size, file );

	XML_SetUserData( parser, this );

	XML_SetElementHandler( parser, &cbBeginElementFormat, &cbEndElementFormat );

	m_parserLevel = 0;
	m_parserState = 0;

	m_tagIds = 0;
	m_attributeIds = 0;
	m_defineIds = 0;


	XML_Status status = XML_ParseBuffer( parser, size, XML_TRUE );

	if( status == XML_STATUS_ERROR )
	{
		fprintf(stderr,
			"%s at line %" XML_FMT_INT_MOD "u\n",
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser)
			);
	}

	XML_ParserReset( parser, NULL );

	fclose( file );
}

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbBeginElementCompile( void *userData, const char *name, const char **attr )
{
	XmlCompile * format = static_cast<XmlCompile*>(userData);

	format->beginElementCompile( name, attr );
}

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbEndElementCompile( void *userData, const char *name )
{
	XmlCompile * format = static_cast<XmlCompile*>(userData);

	format->endElementCompile( name );
}

void XmlCompile::beginElementCompile( const char * _name, const char ** _attr )
{
	char beginNode = 0;
	//начала нового тега
	fwrite( &beginNode, sizeof( beginNode ), 1, m_outputFile );

	TMapTags::iterator it_tag_find = m_tags.find( _name );

	if( it_tag_find == m_tags.end() )
	{
		printf("Invalid compile [tag %s not id]\n"
			, _name
			);
		return;
	}

	Tag & tag = it_tag_find->second;

	//индекс тега
	fwrite( &tag.id, sizeof(tag.id), 1, m_outputFile );

	for (int i = 0; _attr[i]; i += 2) 
	{
		const char * attributeName = _attr[i];

		TMapAttributes::iterator it_attribute_find = m_attributes.find( attributeName );

		if( it_attribute_find == m_attributes.end() )
		{
			printf("Invalid compile [attribute %s not id]\n"
				, attributeName
				);
			return;
		}

		char attributeId = it_attribute_find->second;

		//индекс аттрибута
		fwrite( &attributeId, sizeof( attributeId ), 1, m_outputFile );

		Tag::TMapAttributeTypes::iterator it_tag_attributeType_find = 
			tag.m_attributeTypes.find( attributeId );

		if( it_tag_attributeType_find == tag.m_attributeTypes.end() )
		{
			printf("Invalid compile [tag attribute %d not id]\n"
				, int(attributeId)
				);
			return;
		}

		char typeAttr = it_tag_attributeType_find->second;

		fwrite( &typeAttr, sizeof( typeAttr ), 1, m_outputFile );

		switch( typeAttr )
		{
		case XMLC_TYPE_STRING:
			{
				std::size_t sizeValue = strlen( _attr[i + 1] );

				char chSizeValue = ( sizeValue < 255 ) ? sizeValue : 255;

				fwrite( &chSizeValue, sizeof( chSizeValue ), 1, m_outputFile );

				if( chSizeValue == 255 )
				{
					fwrite( &sizeValue, sizeof( sizeValue ), 1, m_outputFile );
				}

				fwrite( _attr[i + 1], sizeValue, 1, m_outputFile );
			}break;
		case XMLC_TYPE_BOOL:
			{
				char value_char;
				sscanf( _attr[i + 1], "%c", &value_char );

				bool v = (value_char != 0);
				fwrite( &v, sizeof( v ), 1, m_outputFile );
			}break;
		case XMLC_TYPE_INT:
			{
				int v;
				sscanf( _attr[i + 1], "%d", &v );
				fwrite( &v, sizeof( v ), 1, m_outputFile );
			}break;
		case XMLC_TYPE_FLOAT:
			{
				float v;
				sscanf( _attr[i + 1], "%f", &v );
				fwrite( &v, sizeof( v ), 1, m_outputFile );
			}break;
		case XMLC_TYPE_FLOAT2:
			{
				float v[2];
				sscanf( _attr[i + 1], "%f %f", &v[0], &v[1]);
				fwrite( &v, sizeof( v ), 1, m_outputFile );
			}break;

		case XMLC_TYPE_FLOAT4:
			{
				float v[4];
				sscanf( _attr[i + 1], "%f %f %f %f", &v[0], &v[1], &v[2], &v[3] );
				fwrite( &v, sizeof( v ), 1, m_outputFile );
			}break;
		case XMLC_TYPE_MAT3:
			{
				float v[6];
				sscanf( _attr[i + 1], "%f %f %f %f %f %f", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5] );
				fwrite( &v, sizeof( v ), 1, m_outputFile );
			}break;
		case XMLC_TYPE_DEFINE:
			{
				TMapDefines::iterator it_define_find = m_defines.find( _attr[i + 1] );

				if( it_define_find == m_defines.end() )
				{
					printf("Invalid compile [tag attribute define %s not id]\n"
						, _attr[i + 1]
						);
					return;
				}

				char v = it_define_find->second;
				fwrite( &v, sizeof( v ), 1, m_outputFile );
			}break;
		}

	}
}

void XmlCompile::endElementCompile( const char *name )
{
	char endNode = 1;
	fwrite( &endNode, sizeof( endNode ), 1, m_outputFile );
}

void XmlCompile::compile( const std::string & _in, const std::string & _out )
{
	m_inputFile = fopen( _in.c_str(), "rb" );
	m_outputFile = fopen( _out.c_str(), "wb" );

	fseek( m_inputFile, 0, SEEK_END );
	long size = ftell( m_inputFile );

	XML_Parser m_parser = XML_ParserCreate(NULL);

	void * body = XML_GetBuffer( m_parser, size );

	fseek( m_inputFile, 0, SEEK_SET );
	fread( body, 1, size, m_inputFile );

	XML_SetUserData( m_parser, this );

	XML_SetElementHandler( m_parser, &cbBeginElementCompile, &cbEndElementCompile );

	bool done = true;

	XML_Status status = XML_ParseBuffer( m_parser, size, XML_TRUE );

	if( status == XML_STATUS_ERROR )
	{
		fprintf(stderr,
			"%s at line %" XML_FMT_INT_MOD "u\n",
			XML_ErrorString(XML_GetErrorCode(m_parser)),
			XML_GetCurrentLineNumber(m_parser)
			);
	}

	XML_ParserReset( m_parser, NULL );

	fclose( m_outputFile );
	fclose( m_inputFile );
}