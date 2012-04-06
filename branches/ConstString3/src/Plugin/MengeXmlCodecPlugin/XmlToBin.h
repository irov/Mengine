#	pragma once

#	include <map>
#	include <iostream>

#	include "pugixml.hpp"

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

class XmlToBin
{
public:
	bool readProtocol( const wchar_t * _file );
	void writeHeader( const wchar_t * _file );
	bool writeBinary( const wchar_t * _source, const wchar_t * _bin );

	const std::string & getLastError() const;

protected:
	bool writeNodeBinary_( std::ofstream & _stream, const pugi::xml_node & _element );

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

extern "C" bool writeHeader( const wchar_t * _protocol, const wchar_t * _header );
extern "C" bool writeBinary( const wchar_t * _protocol, const wchar_t * _source, const wchar_t * _bin, char * _error );