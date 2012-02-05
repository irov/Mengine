#	pragma once

#	include <map>
#	include <iostream>

struct AttributeXml
{
	size_t id;
	std::string type;
};

typedef std::map<std::string, AttributeXml> TMapAttributes;

struct NodeXml
{
	size_t id;
	TMapAttributes attr;
};

typedef std::map<std::string, NodeXml> TMapNodes;

class TiXmlNode;
class TiXmlElement;

class XmlToBin
{
public:
	bool readProtocol( const char * _file );
	void writeHeader( const char * _file );
	bool writeBinary( const char * _source, const char * _bin );

	const std::string & getLastError();

protected:
	bool writeNodeBinary_( std::ofstream & _stream, TiXmlElement * _element );

protected:
	TMapNodes m_nodes;

	typedef bool (*ValueSerialization)( std::ofstream & _stream, const char * _value );
	typedef std::map<std::string, ValueSerialization> TMapSerialization;

	TMapSerialization m_serialization;

	typedef std::map<std::string, int> TMapSkipAttributes;
	typedef std::map<std::string, TMapSkipAttributes> TMapSkipNodesAttributes;
	TMapSkipNodesAttributes m_setSkipNodesAttributes;

	std::string m_error;
};

extern "C" bool writeHeader( const char * _protocol, const char * _header );
extern "C" bool writeBinary( const char * _protocol, const char * _source, const char * _bin, char * _error );