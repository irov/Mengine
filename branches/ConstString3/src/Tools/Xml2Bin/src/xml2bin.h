#	pragma once

#	include <map>
#	include <iostream>

struct AttributeXml
{
	std::size_t id;
	std::string type;
};

typedef std::map<std::string, AttributeXml> TMapAttributes;

struct NodeXml
{
	std::size_t id;
	TMapAttributes attr;
};

typedef std::map<std::string, NodeXml> TMapNodes;

class TiXmlNode;

class Xml2Bin
{
public:
	bool readProtocol( const char * _file );
	void writeHeader( const char * _file );

	bool writeBinary( const char * _source, const char * _bin );

protected:
	bool writeNodeBinary_( std::ofstream & _stream, TiXmlNode * _base );

protected:
	TMapNodes m_nodes;

	typedef bool (*ValueSerialization)( std::ofstream & _stream, const char * _value );
	typedef std::map<std::string, ValueSerialization> TMapSerialization;

	TMapSerialization m_serialization;
};