#	pragma once

#	include "XmlEnum.h"

#	include <string>
#	include <map>

#	include <stdio.h>

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class XmlTag;

	class XmlCompile
	{
	public:
		XmlCompile();
		~XmlCompile();


	public:
		void load( const String & _fileFormat );
		void compile( const String & _in, const String & _out );

	public:
		XmlTag * tag( XMLTag _tag, const String & _name, XmlTag * _parent = 0 );
		void define( XMLDefine _def, const String & _name );
		void attribute( XMLAttribute _attr, const String & _name );
		void type( XMLType _type, const String & _name );

	public:
		XMLTag getTagId( const String & _name ) const;
		XMLAttribute getAttributeId( const String & _name ) const;
		XMLDefine getDefineId( const String & _name ) const;
		XMLType getTypeId( const String & _name ) const;

	protected:
		//static void cbBeginElementFormat( void *userData, const char *name, const char **attr );
		//static void cbEndElementFormat( void *userData, const char *name );

		//void beginElementFormat( const char *name, const char **attr );
		//void endElementFormat( const char *name );

		static void cbBeginElementCompile( void *userData, const TChar *name, const TChar **attr );
		static void cbEndElementCompile( void *userData, const TChar *name );

		void beginElementCompile( const TChar *name, const TChar **attr );
		void endElementCompile( const TChar *name );

	protected:
		void writeString_( const TChar * _string );

	protected:
		typedef std::map<String, XMLDefine> TMapDefines;
		TMapDefines m_defines;

		typedef std::map<String, XMLAttribute> TMapAttributes;
		TMapAttributes m_attributes;

		typedef std::map<String, XMLType> TMapTypes;
		TMapTypes m_types;

		typedef std::map<String, XMLTag> TMapTags;
		TMapTags m_tags;

		DataStreamInterface * m_inFile;
		OutStreamInterface * m_outFile;

		XmlTag * m_root;

		typedef std::vector<XmlTag*> TXmlTagsLevel;
		TXmlTagsLevel m_tagsLevel;
	};
}