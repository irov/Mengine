#	include "Xml2Bin.h"

#	include "../tinyxml/tinyxml.h"

#	include <sstream>
#	include <fstream>

#	include <time.h>


//////////////////////////////////////////////////////////////////////////
bool Xml2Bin::readProtocol( const char * _file )
{
	TiXmlDocument doc;

	if( doc.LoadFile( _file ) == false )
	{
		printf("%s"
			, doc.ErrorDesc() 
			);

		return false;
	}

	TiXmlNode * protocol = doc.FirstChild();

	if( protocol == 0 )
	{
		return 0;
	}

	std::size_t enumerator = 0;

	for( TiXmlNode * node = protocol->FirstChild(); node; node = protocol->IterateChildren( node ) )
	{
		TiXmlElement * element = node->ToElement();

		if( element == 0 )
		{
			continue;
		}

		const char * NodeType = element->Attribute("NodeType");
		const char * AttrName = element->Attribute("AttrName");
		const char * AttrType = element->Attribute("AttrType");

		NodeXml & nodeXml = m_nodes[NodeType];

		nodeXml.id = ++enumerator;

		if( AttrName == 0 || AttrType == 0 )
		{
			continue;
		}

		AttributeXml & attributeXml = nodeXml.attr[ AttrName ];

		attributeXml.id = ++enumerator;
		attributeXml.type = AttrType;
	}

	doc.Clear();

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Xml2Bin::writeHeader( const char * _file )
{
	std::stringstream ss;

	ss << "#	pragma once" << std::endl;
	ss << std::endl;
	ss << "namespace Menge" << std::endl;
	ss << "{" << std::endl;

	ss << "	namespace Protocol" << std::endl;
	ss << "	{" << std::endl;

	time_t seconds = time(0);
	ss << "		static const int version = " << seconds << ";" << std::endl;

	for( TMapNodes::iterator
		it = m_nodes.begin(),
		it_end = m_nodes.end();
	it != it_end;
	++it )
	{
		ss << "		struct " << it->first << std::endl;
		ss << "		{" << std::endl;
		ss << "			static const int id = " << it->second.id << ";" << std::endl;
		ss << "		};" << std::endl;

		for( TMapAttributes::iterator
			it_attr = it->second.attr.begin(),
			it_attr_end = it->second.attr.end();
		it_attr != it_attr_end;
		++it_attr)
		{
			ss << "		struct " << it->first << "_" << it_attr->first << std::endl;
			ss << "		{" << std::endl;
			ss << "			static const int id = " << it_attr->second.id << ";" << std::endl;
			ss << "			typedef " << it_attr->second.type << " Type;" << std::endl;
			ss << "		};" << std::endl;
		}
	}

	ss << "	}" << std::endl;
	ss << "}" << std::endl;

	std::ofstream fs;
	fs.open( _file );
	fs << ss.str();
	fs.close();
}
//////////////////////////////////////////////////////////////////////////
namespace
{
	template<class T>
	static void s_writeStream( std::ofstream & _stream, T _t )
	{
		_stream.write( (const char *)&_t, sizeof(_t) );
	}

	template<class T>
	static void s_writeStream( std::ofstream & _stream, T * _t, std::streamsize i )
	{
		_stream.write( (const char *)_t, sizeof(*_t)*i );
	}

	static bool s_writeBool( std::ofstream & _stream, const char * _str )
	{
		int valueInt;
		if( sscanf_s( _str, "%d", &valueInt ) != 1)
		{
			return false;
		}

		bool value = (valueInt == 1);

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeInt( std::ofstream & _stream, const char * _str )
	{
		int value;
		if( sscanf_s( _str, "%d", &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeSizet( std::ofstream & _stream, const char * _str )
	{
		size_t value;
		if( sscanf_s( _str, "%u", &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeSizet2( std::ofstream & _stream, const char * _str )
	{
		size_t value[2];
		if( sscanf_s( _str, "%d;%d", &value[0], &value[1] ) != 2)
		{
			return false;
		}

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeUnsignedShort( std::ofstream & _stream, const char * _str )
	{
		unsigned int valueInt;
		if( sscanf_s( _str, "%u", &valueInt ) != 1)
		{
			return false;
		}

		unsigned short value = (unsigned short)valueInt;

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeFloat( std::ofstream & _stream, const char * _str )
	{
		float value;
		if( sscanf_s( _str, "%f", &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeFloat2( std::ofstream & _stream, const char * _str )
	{
		float value[2];
		if( sscanf_s( _str, "%f;%f", &value[0], &value[1] ) != 2)
		{
			return false;
		}

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeFloat4( std::ofstream & _stream, const char * _str )
	{
		float value[4];
		if( sscanf_s( _str, "%f;%f;%f;%f", &value[0], &value[1], &value[2], &value[3] ) != 4)
		{
			return false;
		}

		s_writeStream( _stream, value, 4 );

		return true;
	}

	static bool s_writeFloat6( std::ofstream & _stream, const char * _str )
	{
		float value[6];
		if( sscanf_s( _str 
			, "%f;%f;%f;%f;%f;%f"
			, &value[0], &value[1]
		, &value[2], &value[3] 
		, &value[4], &value[5] ) != 6 )
		{
			return false;
		}

		s_writeStream( _stream, value, 6 );

		return true;
	}

	static bool s_writeString( std::ofstream & _stream, const char * _str )
	{
		std::streamsize size = (std::streamsize)strlen(_str);
		s_writeStream( _stream, size );
		s_writeStream( _stream, _str, size );

		return true;
	}
};
//////////////////////////////////////////////////////////////////////////
bool Xml2Bin::writeBinary( const char * _source, const char * _bin, int _version )
{
	m_serialization["Menge::String"] = &s_writeString;
	m_serialization["Menge::ConstString"] = &s_writeString;
	m_serialization["bool"] = &s_writeBool;
	m_serialization["unsigned short"] = &s_writeUnsignedShort;
	m_serialization["int"] = &s_writeInt;
	m_serialization["float"] = &s_writeFloat;
	m_serialization["size_t"] = &s_writeSizet;	
	m_serialization["Menge::Resolution"] = &s_writeSizet2;
	m_serialization["Menge::ColourValue"] = &s_writeFloat4;
	m_serialization["Menge::Viewport"] = &s_writeFloat4;
	m_serialization["mt::vec2f"] = &s_writeFloat2;
	m_serialization["mt::vec4f"] = &s_writeFloat4;
	m_serialization["mt::mat3f"] = &s_writeFloat6;

	m_setSkipNodesAttributes["File"]["NoAtlas"] = 1;
	m_setSkipNodesAttributes["File"]["NoJPEG"] = 1;
	m_setSkipNodesAttributes["Text"]["MaxSize"] = 1;

	TiXmlDocument doc;

	if( doc.LoadFile( _source ) == false )
	{
		m_error = doc.ErrorDesc();

		return false;
	}

	std::ofstream fs( _bin, std::ios_base::binary );

	char magic_number = 42; 
	s_writeStream( fs, magic_number );
	s_writeStream( fs, _version );

	TiXmlNode * node = doc.FirstChild();

	bool done = true;

	for( TiXmlNode * node = doc.FirstChild(); node; node = doc.IterateChildren( node ) )
	{
		TiXmlElement * element = node->ToElement();

		if( element == 0 )
		{
			continue;
		}

		if( this->writeNodeBinary_( fs, element ) == false )
		{
			done = false;
			break;			 
		}
	}

	fs.close();

	if( done == false )
	{
		remove( _bin );

		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
const std::string & Xml2Bin::getLastError()
{
	return m_error;
}
//////////////////////////////////////////////////////////////////////////
bool Xml2Bin::writeNodeBinary_( std::ofstream & _stream, TiXmlElement * _element )
{
	const char * nodeName = _element->Value();

	TMapNodes::const_iterator it_found = m_nodes.find( nodeName );

	if( it_found == m_nodes.end() )
	{
		std::stringstream ss;
		ss << "Xml2Bin write not found node '" << nodeName << "'";
		m_error = ss.str();

		return false;
	}

	std::size_t id = it_found->second.id;
	s_writeStream( _stream, id );

	std::size_t sizeAttr = 0;

	for( TiXmlAttribute * attr = _element->FirstAttribute(); attr; attr = attr->Next() )
	{
		const char * attrName = attr->Name();

		TMapAttributes::const_iterator it_attr_found = it_found->second.attr.find( attrName );

		if( it_attr_found == it_found->second.attr.end() )
		{
			int skip = m_setSkipNodesAttributes[nodeName][attrName];

			if( skip == 1 )
			{
				continue;
			}

			std::stringstream ss;
			ss << "Xml2Bin write not found attr '" << attrName << "' in node '" << nodeName << "'";
			m_error = ss.str();

			return false;
		}

		++sizeAttr;
	}

	s_writeStream( _stream, sizeAttr );

	for( TiXmlAttribute * attr = _element->FirstAttribute(); attr; attr = attr->Next() )
	{
		const char * attrName = attr->Name();

		TMapAttributes::const_iterator it_attr_found = it_found->second.attr.find( attrName );

		if( it_attr_found == it_found->second.attr.end() )
		{
			int skip = m_setSkipNodesAttributes[nodeName][attrName];

			if( skip == 1 )
			{
				continue;
			}

			std::stringstream ss;
			ss << "Xml2Bin write not found attr '" << attrName << "' in node '" << nodeName << "'";
			m_error = ss.str();

			return false;
		}

		std::size_t id = it_attr_found->second.id;

		s_writeStream( _stream, id );

		const std::string & type = it_attr_found->second.type;

		TMapSerialization::iterator it_found = m_serialization.find(type);

		if( it_found == m_serialization.end() )
		{
			std::stringstream ss;
			ss << "Xml2Bin write not found serializator '" << type << " for attr " << attrName << "' in node '" << nodeName << "'";
			m_error = ss.str();

			return false;
		}

		const char * attrValue = attr->Value();

		if( (*it_found->second)( _stream, attrValue ) == false )
		{
			std::stringstream ss;
			ss << "Xml2Bin write invalid serializator '" << type << " for attr " << attrName << "' in node '" << nodeName << "'";
			m_error = ss.str();

			return false;
		}
	}

	std::size_t sizeNode = 0;

	for( TiXmlNode * node = _element->FirstChild(); node; node = _element->IterateChildren( node ) )
	{
		TiXmlElement * element = node->ToElement();

		if( element == 0 )
		{
			continue;
		}

		++sizeNode;
	}

	s_writeStream( _stream, sizeNode );

	for( TiXmlNode * node = _element->FirstChild(); node; node = _element->IterateChildren( node ) )
	{
		TiXmlElement * element = node->ToElement();

		if( element == 0 )
		{
			continue;
		}

		if( this->writeNodeBinary_( _stream, element ) == false )
		{
			return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
extern "C" bool writeHeader( const char * _protocol, const char * _header )
{
	Xml2Bin x2b;

	if( x2b.readProtocol( _protocol ) == false )
	{
		return false;
	}

	x2b.writeHeader( _header );

	return true;
}
//////////////////////////////////////////////////////////////////////////
extern "C" bool writeBinary( const char * _protocol, const char * _source, const char * _bin, int _version, char * _error )
{
	Xml2Bin x2b;

	if( x2b.readProtocol( _protocol ) == false )
	{
		const std::string & error = x2b.getLastError();
		strcpy_s( _error, 255, error.c_str() );

		return false;
	}

	if( x2b.writeBinary( _source, _bin, _version ) == false )
	{
		const std::string & error = x2b.getLastError();
		strcpy_s( _error, 255, error.c_str() );

		return false;
	}

	return true;
}