#	include "XmlCompile.h"

#	ifdef MENGE_UNICODE
#	define XML_UNICODE_WCHAR_T
#	endif

#	include "expat/lib/expat.h"

#	include "FileEngine.h"

#	include "XmlFormat.h"
#	include "XmlTag.h"

#	include "LogEngine.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

namespace Menge
{
	XmlCompile::XmlCompile()
		: m_inFile(0)
		, m_outFile(0)
	{
		m_root = new XmlTag( TAG_BEGIN, MENGE_TEXT("Root"), 0 );

		m_tagsLevel.push_back( m_root );
	}
	//////////////////////////////////////////////////////////////////////////
	XmlCompile::~XmlCompile()
	{
		delete m_root;
	}
	//////////////////////////////////////////////////////////////////////////
	XmlTag * XmlCompile::tag( XMLTag _tag, const String & _name, XmlTag * _parent )
	{
		if( _parent == 0 )
		{
			_parent = m_root;
		}

		XmlTag * xmlTag = new XmlTag( _tag, _name, _parent );

		m_tags[ _name ] = _tag;

		return xmlTag;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCompile::define( XMLDefine _def, const String & _name )
	{
		m_defines[ _name ] = _def;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCompile::attribute( XMLAttribute _attr, const String & _name )
	{
		m_attributes[ _name ] = _attr;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCompile::type( XMLType _type, const String & _name )
	{
		m_types[ _name ] = _type;
	}
	//////////////////////////////////////////////////////////////////////////
	XMLTag XmlCompile::getTagId( const String & _name ) const
	{
		TMapTags::const_iterator it_tag_find = m_tags.find( _name );

		if( it_tag_find == m_tags.end() )
		{
			return TAG_BEGIN;
		}

		return it_tag_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	XMLAttribute XmlCompile::getAttributeId( const String & _name ) const
	{
		TMapAttributes::const_iterator it_attr_find = m_attributes.find( _name );

		if( it_attr_find == m_attributes.end() )
		{
			return ATTR_BEGIN;
		}

		return it_attr_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	XMLDefine XmlCompile::getDefineId( const String & _name ) const
	{
		TMapDefines::const_iterator it_define_find = m_defines.find( _name );

		if( it_define_find == m_defines.end() )
		{
			return DEF_BEGIN;
		}

		return it_define_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	XMLType XmlCompile::getTypeId( const String & _name ) const
	{
		TMapTypes::const_iterator it_type_find = m_types.find( _name );

		if( it_type_find == m_types.end() )
		{
			return TYPE_BEGIN;
		}

		return it_type_find->second;
	}
	////////////////////////////////////////////////////////////////////////////
	//void XmlCompile::cbBeginElementFormat( void *userData, const char *name, const char **attr )
	//{
	//	XmlCompile * format = static_cast<XmlCompile*>(userData);

	//	format->beginElementFormat( name, attr );
	//}

	////////////////////////////////////////////////////////////////////////////
	//void XmlCompile::cbEndElementFormat( void *userData, const char *name )
	//{
	//	XmlCompile * format = static_cast<XmlCompile*>(userData);

	//	format->endElementFormat( name );
	//}

	//void XmlCompile::beginElementFormat( const char * _name, const char ** _attr )
	//{
	//	if( m_parserLevel == 1 )
	//	{
	//		if( strcmp( _name, "Tag" ) == 0 )
	//		{
	//			m_parserState = 1;
	//		}
	//		else if( strcmp( _name, "Attribute" ) == 0 )
	//		{
	//			m_parserState = 2;
	//		}
	//		else if( strcmp( _name, "Define" ) == 0 )
	//		{
	//			m_parserState = 3;
	//		}
	//	}
	//	else if( m_parserLevel > 1 )
	//	{
	//		if( m_parserState == 1 )
	//		{
	//			Tag & tag = m_tags[_name];

	//			tag.id = ++m_tagIds;

	//			for (int i = 0; _attr[i]; i += 2) 
	//			{
	//				TMapAttributes::iterator it_attribute_find = m_attributes.find( _attr[i] );

	//				if( it_attribute_find == m_attributes.end() )
	//				{
	//					printf("Xml Compiler Format load is failed [attribute '%s' not id\n"
	//						, _attr[i]
	//					);
	//					return;
	//				}

	//				char attrTypeId = it_attribute_find->second;

	//				char & attrId = tag.m_attributeTypes[ attrTypeId ];

	//				if( strcmp( _attr[i + 1], "String" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_STRING;
	//				}
	//				else if( strcmp( _attr[i + 1], "Bool" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_BOOL;
	//				}
	//				else if( strcmp( _attr[i + 1], "Int" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_INT;
	//				}
	//				else if( strcmp( _attr[i + 1], "Float" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_FLOAT;
	//				}
	//				else if( strcmp( _attr[i + 1], "Float2" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_FLOAT2;
	//				}
	//				else if( strcmp( _attr[i + 1], "Float4" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_FLOAT4;
	//				}
	//				else if( strcmp( _attr[i + 1], "Mat3" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_MAT3;
	//				}
	//				else if( strcmp( _attr[i + 1], "Define" ) == 0 )
	//				{
	//					attrId = XMLC_TYPE_DEFINE;
	//				}
	//			}
	//		}
	//		else if( m_parserState == 2 )
	//		{
	//			char & attribute = m_attributes[ _name ];
	//			attribute = ++m_attributeIds;
	//		}
	//		else if( m_parserState == 3 )
	//		{
	//			char & define = m_defines[ _name ];
	//			define = ++m_defineIds;
	//		}
	//	}

	//	++m_parserLevel;
	//}

	//void XmlCompile::endElementFormat( const char *name )
	//{
	//	--m_parserLevel;
	//}

	//void XmlCompile::load( const std::string & _fileFormat )
	//{
	//	FILE * file = fopen( _fileFormat.c_str(), "rb" );

	//	fseek( file, 0, SEEK_END );
	//	long size = ftell( file );

	//	XML_Parser parser = XML_ParserCreate(NULL);

	//	void * body = XML_GetBuffer( parser, size );

	//	fseek( file, 0, SEEK_SET );
	//	fread( body, 1, size, file );

	//	XML_SetUserData( parser, this );

	//	XML_SetElementHandler( parser, &XmlCompile::cbBeginElementFormat, &XmlCompile::cbEndElementFormat );

	//	m_parserLevel = 0;
	//	m_parserState = 0;

	//	m_tagIds = 0;
	//	m_attributeIds = 0;
	//	m_defineIds = 0;


	//	XML_Status status = XML_ParseBuffer( parser, size, XML_TRUE );

	//	if( status == XML_STATUS_ERROR )
	//	{
	//		fprintf(stderr,
	//			"%s at line %" XML_FMT_INT_MOD "u\n",
	//			XML_ErrorString(XML_GetErrorCode(parser)),
	//			XML_GetCurrentLineNumber(parser)
	//			);
	//	}

	//	XML_ParserReset( parser, NULL );

	//	fclose( file );
	//}

	//////////////////////////////////////////////////////////////////////////
	void XmlCompile::cbBeginElementCompile( void *userData, const TChar *name, const TChar **attr )
	{
		XmlCompile * format = static_cast<XmlCompile*>(userData);

		format->beginElementCompile( name, attr );
	}

	//////////////////////////////////////////////////////////////////////////
	void XmlCompile::cbEndElementCompile( void *userData, const TChar *name )
	{
		XmlCompile * format = static_cast<XmlCompile*>(userData);

		format->endElementCompile( name );
	}

	void XmlCompile::beginElementCompile( const TChar * _tagName, const TChar ** _attr )
	{
		char beginNode = 0;
		//начала нового тега
		m_outFile->write( &beginNode, sizeof( beginNode ) );

		XMLTag tagId = this->getTagId( _tagName );

		if(	tagId == TAG_BEGIN )
		{
			MENGE_LOG( MENGE_TEXT("XmlCompile invalid compile - tag id '%s'")
				, _tagName
				);

			return;
		}

		XmlTag * lastTag = m_tagsLevel.back();

		XmlTag * tag = lastTag->getTag( tagId );

		if( tag == 0 )
		{
			MENGE_LOG( MENGE_TEXT("XmlCompile invalid compile - tag '%s'")
				, _tagName
				);

			return;
		}

		m_tagsLevel.push_back( tag );

		//индекс тега
		XMLTag id = tag->getId();
		char id_w = id;
		m_outFile->write( &id_w, sizeof(id_w) );

		if( id == TAG_VALUE )
		{
			writeString_( _tagName );
		}

		for (int i = 0; _attr[i]; i += 2)
		{
			const TChar * attributeName = _attr[i];

			XMLAttribute attrId = this->getAttributeId( attributeName );

			const TagAttribute & tagAttr = tag->getAttribute( attrId );

			if( tagAttr.valid == false )
			{
				MENGE_LOG( MENGE_TEXT("XmlCompile invalid compile - attr '%s'")
					, attributeName
					);

				return;
			}

			char attrId_w = tagAttr.attrId;
			m_outFile->write( &attrId_w, sizeof( attrId_w ) );

			char tagAttrType_w = tagAttr.attrType;
			m_outFile->write( &tagAttrType_w, sizeof( tagAttrType_w ) );

			const TChar * attrValue = _attr[i + 1];

			switch( tagAttr.attrType )
			{
			case TYPE_String:
				{
					writeString_( attrValue );
				}break;
			case TYPE_Bool:
				{
					char value_char;
					STDSSCANF( attrValue, MENGE_TEXT("%c"), &value_char );

					bool v = (value_char != 0);
					m_outFile->write( &v, sizeof( v ) );
				}break;
			case TYPE_Int:
				{
					int v;
					STDSSCANF( attrValue, MENGE_TEXT("%d"), &v );
					m_outFile->write( &v, sizeof( v ) );
				}break;
			case TYPE_Float:
				{
					float v;
					STDSSCANF( attrValue, MENGE_TEXT("%f"), &v );
					m_outFile->write( &v, sizeof( v ) );
				}break;
			case TYPE_Float2:
				{
					float v[2];
					STDSSCANF( attrValue, MENGE_TEXT("%f %f"), &v[0], &v[1]);
					m_outFile->write( &v, sizeof( v ) );
				}break;

			case TYPE_Float4:
				{
					float v[4];
					STDSSCANF( attrValue, MENGE_TEXT("%f %f %f %f"), &v[0], &v[1], &v[2], &v[3] );
					m_outFile->write( &v, sizeof( v ) );
				}break;
			case TYPE_Mat3:
				{
					float v[6];
					STDSSCANF( attrValue, MENGE_TEXT("%f %f %f %f %f %f"), &v[0], &v[1], &v[2], &v[3], &v[4], &v[5] );
					m_outFile->write( &v, sizeof( v ) );
				}break;
			case TYPE_Define:
				{
					XMLDefine defId = this->getDefineId( attrValue );

					if( defId == DEF_BEGIN )
					{
						MENGE_LOG( MENGE_TEXT("Invalid compile [tag attribute define %s not id]\n")
							, attrValue
						);
						return;
					}

					char defId_w = defId;
					m_outFile->write( &defId_w, sizeof( defId_w ) );
				}break;
			}
		}
	}
	void XmlCompile::endElementCompile( const TChar *name )
	{
		char endNode = 1;
		m_outFile->write( &endNode, sizeof( endNode ) );

		m_tagsLevel.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCompile::writeString_( const TChar * _string )
	{
		std::size_t sizeValue = STDSTRLEN( _string );

		char chSizeValue = ( sizeValue < 255 ) ? char(sizeValue) : 255;

		m_outFile->write( &chSizeValue, sizeof( chSizeValue ) );

		if( chSizeValue == 255 )
		{
			m_outFile->write( &sizeValue, sizeof( sizeValue ) );
		}

		m_outFile->write( _string, sizeValue );
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCompile::compile( const String & _in, const String & _out )
	{
		m_inFile = Holder<FileEngine>::hostage()
			->openFile( _in );

		m_outFile = Holder<FileEngine>::hostage()
			->openOutStream( _out, true );

		std::streamsize size = m_inFile->size();

		XML_Parser parser = XML_ParserCreate(NULL);

		void * body = XML_GetBuffer( parser, size );

		m_inFile->read( body, size );

		XML_SetUserData( parser, this );

		XML_SetElementHandler( parser, &XmlCompile::cbBeginElementCompile, &XmlCompile::cbEndElementCompile );

		bool done = true;

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

		Holder<FileEngine>::hostage()
			->closeStream( m_inFile );

		Holder<FileEngine>::hostage()
			->closeOutStream( m_outFile );
	}
}