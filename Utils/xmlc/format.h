#	pragma once

#	include <string>
#	include <map>

class XmlcFormat
{
public:
	void load( const std::string & _fileFormat );

	void parse( const std::string & _in, const std::string & _out );

public:
	void beginElementFormat( const char *name, const char **attr );
	void endElementFormat( const char *name );

protected:
	int m_parserLevel;
	int m_parserState;

	struct Tag
	{
		char id;
		
		typedef std::map<std::string, char> TMapAttributeTypes;
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
};