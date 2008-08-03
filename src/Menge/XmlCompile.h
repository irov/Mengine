#	pragma once

#	include <string>
#	include <map>

#	include <stdio.h>

class XmlParser
{
public:
	void beginTag( char _type );
};

class XmlCompile
{
public:
	void load( const std::string & _fileFormat );

	void compile( const std::string & _in, const std::string & _out );

	//void parse( )

protected:
	static void cbBeginElementFormat( void *userData, const char *name, const char **attr );
	static void cbEndElementFormat( void *userData, const char *name );

	static void cbBeginElementCompile( void *userData, const char *name, const char **attr );
	static void cbEndElementCompile( void *userData, const char *name );

	void beginElementFormat( const char *name, const char **attr );
	void endElementFormat( const char *name );

	void beginElementCompile( const char *name, const char **attr );
	void endElementCompile( const char *name );
	
protected:
	int m_parserLevel;
	int m_parserState;

	struct Tag
	{
		char id;
		
		typedef std::map<char, char> TMapAttributeTypes;
		TMapAttributeTypes m_attributeTypes;
	};

	typedef std::map<std::string, Tag> TMapTags;
	TMapTags m_tags;

	char m_tagIds;

	typedef std::map<std::string, char> TMapAttributes;
	TMapAttributes m_attributes;

	char m_attributeIds;

	typedef std::map<std::string, char> TMapDefines;
	TMapDefines m_defines;

	char m_defineIds;

	FILE * m_inputFile;
	FILE * m_outputFile;
};