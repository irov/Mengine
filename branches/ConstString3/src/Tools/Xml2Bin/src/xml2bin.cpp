#	include "Xml2Bin.h"

#	include "../tinyxml/tinyxml.h"

#	include <sstream>
#	include <fstream>


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
		if( sscanf_s( "%d", _str, &valueInt ) != 1)
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
		if( sscanf_s( "%d", _str, &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeSizet( std::ofstream & _stream, const char * _str )
	{
		size_t value;
		if( sscanf_s( "%u", _str, &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeSizet2( std::ofstream & _stream, const char * _str )
	{
		size_t value[2];
		if( sscanf_s( "%d;%d", _str, &value[0], &value[1] ) != 2)
		{
			return false;
		}

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeUnsignedShort( std::ofstream & _stream, const char * _str )
	{
		unsigned int valueInt;
		if( sscanf_s( "%u", _str, &valueInt ) != 1)
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
		if( sscanf_s( "%f", _str, &value ) != 1)
		{
			return false;
		}

		s_writeStream( _stream, value );

		return true;
	}

	static bool s_writeFloat2( std::ofstream & _stream, const char * _str )
	{
		float value[2];
		if( sscanf_s( "%f;%f", _str, &value[0], &value[1] ) != 2)
		{
			return false;
		}

		s_writeStream( _stream, value, 2 );

		return true;
	}

	static bool s_writeFloat4( std::ofstream & _stream, const char * _str )
	{
		float value[4];
		if( sscanf_s( "%f;%f;%f;%f", _str, &value[0], &value[1], &value[2], &value[3] ) != 4)
		{
			return false;
		}

		s_writeStream( _stream, value, 4 );

		return true;
	}

	static bool s_writeFloat12( std::ofstream & _stream, const char * _str )
	{
		float value[12];
		if( sscanf_s( "%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f"
			, _str
			, &value[0], &value[1], &value[2], &value[3] 
		, &value[4], &value[5], &value[6], &value[7]
		, &value[8], &value[9], &value[10], &value[11] ) != 12 )
		{
			return false;
		}

		s_writeStream( _stream, value, 12 );

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
bool Xml2Bin::writeBinary( const char * _source, const char * _bin )
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
	m_serialization["mt::mat3f"] = &s_writeFloat12;	

	TiXmlDocument doc;

	if( doc.LoadFile( _source ) == false )
	{
		printf("%s"
			, doc.ErrorDesc() 
			);

		return false;
	}

	std::ofstream fs( _bin, std::ios_base::binary );	

	TiXmlNode * node = doc.FirstChild();

	if( this->writeNodeBinary_( fs, node ) == false )
	{
		return false;			 
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Xml2Bin::writeNodeBinary_( std::ofstream & _stream, TiXmlNode * _base )
{
	TiXmlElement * element = _base->ToElement();

	if( element == 0 )
	{
		return false;
	}

	const char * value = element->Value();

	TMapNodes::const_iterator it_found = m_nodes.find( value );

	if( it_found == m_nodes.end() )
	{
		return false;
	}

	s_writeStream( _stream, it_found->second.id );
	s_writeStream( _stream, it_found->second.attr.size() );

	for( TiXmlAttribute * attr = element->FirstAttribute(); attr; attr = attr->Next() )
	{
		const char * attrName = attr->Name();

		TMapAttributes::const_iterator it_attr_found = it_found->second.attr.find( attrName );

		if( it_attr_found == it_found->second.attr.end() )
		{
			return false;
		}

		s_writeStream( _stream, it_attr_found->second.id );

		const std::string & type = it_attr_found->second.type;

		TMapSerialization::iterator it_found = m_serialization.find(type);

		if( it_found == m_serialization.end() )
		{
			return false;
		}

		const char * attrValue = attr->Value();

		(*it_found->second)( _stream, attrValue );
	}

	std::size_t subNode = 0;

	for( TiXmlNode * node = _base->FirstChild(); node; node = _base->IterateChildren( node ) )
	{
		TiXmlElement * element = node->ToElement();

		if( element == 0 )
		{
			continue;
		}

		++subNode;
	}

	s_writeStream( _stream, subNode );

	for( TiXmlNode * node = _base->FirstChild(); node; node = _base->IterateChildren( node ) )
	{
		TiXmlElement * element = node->ToElement();

		if( element == 0 )
		{
			continue;
		}

		if( this->writeNodeBinary_( _stream, node ) == false )
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
extern "C" bool writeBinary( const char * _protocol, const char * _source, const char * _bin )
{
	Xml2Bin x2b;

	if( x2b.readProtocol( _protocol ) == false )
	{
		return false;
	}

	if( x2b.writeBinary( _source, _bin ) == false )
	{
		return false;
	}

	return true;
}
