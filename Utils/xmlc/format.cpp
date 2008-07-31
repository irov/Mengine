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

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbBeginElementFormat( void *userData, const char *name, const char **attr )
{
	XmlcFormat * format = static_cast<XmlcFormat*>(userData);

	format->beginElementFormat( name, attr );
}

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbEndElementFormat( void *userData, const char *name )
{
	XmlcFormat * format = static_cast<XmlcFormat*>(userData);

	format->endElementFormat( name );
}


void XmlcFormat::load( const std::string & _fileFormat )
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

void XmlcFormat::parse( const std::string & _in, const std::string & _out )
{
	FILE * in_file = fopen( _in.c_str(), "rb" );
}

void XmlcFormat::beginElementFormat( const char * _name, const char ** _attr )
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
				char & attrId = tag.m_attributeTypes[ _attr[i] ];

				if( strcmp( _attr[i + 1], "String" ) == 0 )
				{
					attrId = 1;
				}
				else if( strcmp( _attr[i + 1], "Bool" ) == 0 )
				{
					attrId = 2;
				}
				else if( strcmp( _attr[i + 1], "Int" ) == 0 )
				{
					attrId = 3;
				}
				else if( strcmp( _attr[i + 1], "Float" ) == 0 )
				{
					attrId = 4;
				}
				else if( strcmp( _attr[i + 1], "Float2" ) == 0 )
				{
					attrId = 5;
				}
				else if( strcmp( _attr[i + 1], "Float4" ) == 0 )
				{
					attrId = 6;
				}
				else if( strcmp( _attr[i + 1], "Mat3" ) == 0 )
				{
					attrId = 7;
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

void XmlcFormat::endElementFormat( const char *name )
{
	--m_parserLevel;
}