#	include <stdio.h>

#	include <map>
#	include <string>

#	include "expat-2.0.1/lib/expat.h"


#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

FILE * xmlc_file = 0;
FILE * xml_file = 0;

std::map<std::string, short> dict;
std::map<std::string, std::pair<char,char> > attrType;

//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbBeginElement( void *userData, const char *name, const char **attr )
{
	char beginNode = 0;
	fwrite( &beginNode, sizeof( beginNode ), 1, xmlc_file );

	char idNode = dict[name];

	fwrite( &idNode, sizeof(idNode), 1, xmlc_file );

	for (int i = 0; attr[i]; i += 2) 
	{
		std::pair<char,char> idAttr = attrType[ attr[i] ];
		fwrite( &idAttr.first, sizeof( idAttr.first ), 1, xmlc_file );

		char typeAttr = idAttr.second;
		fwrite( &typeAttr, sizeof( typeAttr ), 1, xmlc_file );

		switch( typeAttr )
		{
		case 0:
			{
				std::size_t sizeValue = strlen( attr[i + 1] );

				char chSizeValue = ( sizeValue < 255 ) ? sizeValue : 255;

				fwrite( &chSizeValue, sizeof( chSizeValue ), 1, xmlc_file );

				if( chSizeValue == 255 )
				{
					fwrite( &sizeValue, sizeof( sizeValue ), 1, xmlc_file );
				}

				fwrite( attr[i + 1], sizeValue, 1, xmlc_file );
			}break;
		case 1:
			{
				float value;
				sscanf( attr[i + 1], "%f", &value );
				fwrite( &value, sizeof( value ), 1, xmlc_file );
			}break;
		}

	}
}
//////////////////////////////////////////////////////////////////////////
static void XMLCALL cbEndElement( void *userData, const char *name )
{
	char endNode = 1;
	fwrite( &endNode, sizeof( endNode ), 1, xmlc_file );
}

int main()
{
	dict["Node"] = 0;
	dict["Name"] = 1;
	dict["Type"] = 2;
	dict["Resource"] = 3;
	dict["Size"] = 4;
	dict["Scale"] = 4;	
	dict["Main"] = 5;
	dict["ImageMap"] = 6;
	dict["Transformation"] = 7;
	dict["CenterAlign"] = 8;
	dict["Point"] = 9;

	attrType["File"] = std::make_pair( 0, 0 );
	attrType["Value"] = std::make_pair( 1, 0 );
	attrType["Name"] = std::make_pair( 2, 0 );

	xmlc_file = fopen( "test.xmlc", "wb" );
	xml_file = fopen( "test.xml", "rb" );

	fseek( xml_file, 0, SEEK_END );
	long size = ftell( xml_file );

	XML_Parser m_parser = XML_ParserCreate(NULL);

	void * body = XML_GetBuffer( m_parser, size );

	fseek( xml_file, 0, SEEK_SET );
	fread( body, 1, size, xml_file );

	XML_SetElementHandler( m_parser, cbBeginElement, cbEndElement );

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

	fclose( xml_file );
	fclose( xmlc_file );
}
