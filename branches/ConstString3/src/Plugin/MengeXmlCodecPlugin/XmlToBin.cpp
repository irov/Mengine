//#	include "XmlToBin.h"
//
//#	include <sstream>
//#	include <fstream>
//
//#	include <vector>
//
//#	include <time.h>
//
//#	include <windows.h>
//
////////////////////////////////////////////////////////////////////////////
//bool XmlToBin::readProtocol( const wchar_t * _file )
//{
//	//TiXmlDocument doc;
//	pugi::xml_document doc;
//
//	pugi::xml_parse_result result = doc.load_file( _file );
//
//	if( result == false )
//	{
//		printf("%s"
//			, result.description()
//			);
//
//		return false;
//	}
//
//	pugi::xml_node root = doc.document_element();
//
//	//TiXmlNode * protocol = doc.FirstChild();
//
//	if( root.empty() == true )
//	{
//		return 0;
//	}
//
//	size_t enumerator = 0;
//
//	for( pugi::xml_node::iterator
//		it = root.begin(),
//		it_end = root.end();
//	it != it_end;
//	++it )
//	{
//		const pugi::xml_node & element = *it;
//
//		//element->attribute()
//
//		pugi::xml_attribute NodeType = element.attribute("NodeType");
//		pugi::xml_attribute AttrName = element.attribute("AttrName");
//		pugi::xml_attribute AttrType = element.attribute("AttrType");
//
//		NodeXml & nodeXml = m_nodes[NodeType.value()];
//
//		nodeXml.id = ++enumerator;
//
//		if( AttrName.empty() == true || AttrType.empty() == true )
//		{
//			continue;
//		}
//
//		AttributeXml & attributeXml = nodeXml.attr[ AttrName.value() ];
//
//		attributeXml.id = ++enumerator;
//		attributeXml.type = AttrType.value();
//	}
//	
//	//for( TiXmlNode * node = protocol->FirstChild(); node; node = protocol->IterateChildren( node ) )
//	//{
//	//	TiXmlElement * element = node->ToElement();
//
//	//	if( element == 0 )
//	//	{
//	//		continue;
//	//	}
//
//	//	const char * NodeType = element->Attribute("NodeType");
//	//	const char * AttrName = element->Attribute("AttrName");
//	//	const char * AttrType = element->Attribute("AttrType");
//
//	//	NodeXml & nodeXml = m_nodes[NodeType];
//
//	//	nodeXml.id = ++enumerator;
//
//	//	if( AttrName == 0 || AttrType == 0 )
//	//	{
//	//		continue;
//	//	}
//
//	//	AttributeXml & attributeXml = nodeXml.attr[ AttrName ];
//
//	//	attributeXml.id = ++enumerator;
//	//	attributeXml.type = AttrType;
//	//}
//
//	//doc.Clear();
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//void XmlToBin::writeHeader( const wchar_t * _file )
//{
//	std::stringstream ss;
//
//	ss << "#	pragma once" << std::endl;
//	ss << std::endl;
//	ss << "namespace Menge" << std::endl;
//	ss << "{" << std::endl;
//
//	ss << "	namespace Protocol" << std::endl;
//	ss << "	{" << std::endl;
//
//	time_t seconds = time(0);
//	ss << "		static const int version = " << seconds << ";" << std::endl;
//
//	for( TMapNodes::iterator
//		it = m_nodes.begin(),
//		it_end = m_nodes.end();
//	it != it_end;
//	++it )
//	{
//		ss << "		struct " << it->first << std::endl;
//		ss << "		{" << std::endl;
//		ss << "			static const int id = " << it->second.id << ";" << std::endl;
//		ss << "		};" << std::endl;
//
//		for( TMapAttributes::iterator
//			it_attr = it->second.attr.begin(),
//			it_attr_end = it->second.attr.end();
//		it_attr != it_attr_end;
//		++it_attr)
//		{
//			ss << "		struct " << it->first << "_" << it_attr->first << std::endl;
//			ss << "		{" << std::endl;
//			ss << "			static const int id = " << it_attr->second.id << ";" << std::endl;
//			ss << "			typedef " << it_attr->second.type << " Type;" << std::endl;
//			ss << "		};" << std::endl;
//		}
//	}
//
//	ss << "	}" << std::endl;
//	ss << "}" << std::endl;
//
//	std::ofstream fs;
//	fs.open( _file );
//	fs << ss.str();
//	fs.close();
//}
////////////////////////////////////////////////////////////////////////////
//namespace
//{
//	template<class T>
//	static void s_writeStream( std::ofstream & _stream, T _t )
//	{
//		_stream.write( (const char *)&_t, sizeof(_t) );
//	}
//
//	template<class T>
//	static void s_writeStreamCount( std::ofstream & _stream, T * _t, size_t _count )
//	{
//		_stream.write( (const char *)_t, sizeof(*_t) * _count );
//	}
//
//	static bool s_writeBool( std::ofstream & _stream, const char * _str )
//	{
//		int valueInt;
//		if( sscanf_s( _str, "%d", &valueInt ) != 1)
//		{
//			return false;
//		}
//
//		bool value = (valueInt == 1);
//
//		s_writeStream( _stream, value );
//
//		return true;
//	}
//
//	static bool s_writeInt( std::ofstream & _stream, const char * _str )
//	{
//		int value;
//		if( sscanf_s( _str, "%d", &value ) != 1)
//		{
//			return false;
//		}
//
//		s_writeStream( _stream, value );
//
//		return true;
//	}
//
//	static bool s_writeSizet( std::ofstream & _stream, const char * _str )
//	{
//		size_t value;
//		if( sscanf_s( _str, "%u", &value ) != 1)
//		{
//			return false;
//		}
//
//		s_writeStream( _stream, value );
//
//		return true;
//	}
//
//	static bool s_writeSizet2( std::ofstream & _stream, const char * _str )
//	{
//		size_t value[2];
//		if( sscanf_s( _str, "%d;%d", &value[0], &value[1] ) != 2)
//		{
//			return false;
//		}
//
//		s_writeStreamCount( _stream, value, 2 );
//
//		return true;
//	}
//
//	static bool s_writeUnsignedShort( std::ofstream & _stream, const char * _str )
//	{
//		unsigned int valueInt;
//		if( sscanf_s( _str, "%u", &valueInt ) != 1)
//		{
//			return false;
//		}
//
//		unsigned short value = (unsigned short)valueInt;
//
//		s_writeStream( _stream, value );
//
//		return true;
//	}
//
//	static bool s_writeFloat( std::ofstream & _stream, const char * _str )
//	{
//		float value;
//		if( sscanf_s( _str, "%f", &value ) != 1)
//		{
//			return false;
//		}
//
//		s_writeStream( _stream, value );
//
//		return true;
//	}
//
//	static bool s_writeFloat2( std::ofstream & _stream, const char * _str )
//	{
//		float value[2];
//		if( sscanf_s( _str, "%f;%f", &value[0], &value[1] ) != 2 )
//		{
//			if( sscanf_s( _str, "%f %f", &value[0], &value[1] ) != 2 )
//			{
//				return false;
//			}
//		}
//
//		s_writeStreamCount( _stream, value, 2 );
//
//		return true;
//	}
//
//	static bool s_writeFloat3( std::ofstream & _stream, const char * _str )
//	{
//		float value[3];
//		if( sscanf_s( _str, "%f;%f;%f", &value[0], &value[1], &value[2] ) != 3 )
//		{
//			return false;
//		}
//
//		s_writeStreamCount( _stream, value, 3 );
//
//		return true;
//	}
//
//	namespace detail
//	{
//		typedef std::vector<std::string> TVectorString;
//		//////////////////////////////////////////////////////////////////////////
//		void split( TVectorString & _outStrings, const std::string & _str, bool _trimDelims, const std::string & _delims /*= "\t\n "*/, unsigned int _maxSplits /*= 0 */ )
//		{
//			// Pre-allocate some space for performance
//			_outStrings.reserve(_maxSplits ? _maxSplits+1 : 10);    // 10 is guessed capacity for most case
//
//			unsigned int numSplits = 0;
//
//			// Use STL methods 
//			size_t start, pos;
//			start = 0;
//			do 
//			{
//				pos = _str.find_first_of(_delims, start);
//				/*if (pos == start)
//				{
//				// Do nothing
//				start = pos + 1;
//				}
//				else */if (pos == std::string::npos || (_maxSplits && numSplits == _maxSplits))
//				{
//					// Copy the rest of the string
//					_outStrings.push_back( _str.substr(start) );
//					break;
//				}
//				else
//				{
//					// Copy up to delimiter
//					_outStrings.push_back( _str.substr(start, pos - start) );
//					start = pos + 1;
//				}
//				// parse up to next real data
//				if( _trimDelims == true )
//				{
//					start = _str.find_first_not_of(_delims, start);
//				}
//				++numSplits;
//
//			} while (pos != std::string::npos);
//		}
//	}
//
//	static bool s_writeVector( std::ofstream & _stream, const char * _str )
//	{
//		detail::TVectorString vs;
//		detail::split( vs, _str, false, ",", 0 );
//
//		detail::TVectorString::size_type size = vs.size();
//		s_writeStream( _stream, size );
//
//		for( detail::TVectorString::iterator
//			it = vs.begin(),
//			it_end = vs.end();
//		it != it_end;
//		++it )
//		{
//			unsigned int value;
//			
//			if( sscanf_s( it->c_str(), "%u", &value ) != 1)
//			{
//				return false;
//			}
//
//			s_writeStream<unsigned short>( _stream, value );
//		}
//
//		return true;
//	}
//
//	static bool s_writeFloat4( std::ofstream & _stream, const char * _str )
//	{
//		float value[4];
//		if( sscanf_s( _str, "%f;%f;%f;%f", &value[0], &value[1], &value[2], &value[3] ) != 4)
//		{
//			if( sscanf_s( _str, "%f %f %f %f", &value[0], &value[1], &value[2], &value[3] ) != 4)
//			{
//				return false;
//			}
//		}
//
//		s_writeStreamCount( _stream, value, 4 );
//
//		return true;
//	}
//
//	static bool s_writeFloat6( std::ofstream & _stream, const char * _str )
//	{
//		float value[6];
//		if( sscanf_s( _str 
//			, "%f;%f;%f;%f;%f;%f"
//			, &value[0], &value[1]
//		, &value[2], &value[3] 
//		, &value[4], &value[5] ) != 6 )
//		{
//			return false;
//		}
//
//		s_writeStreamCount( _stream, value, 6 );
//
//		return true;
//	}
//
//	static bool s_writeString( std::ofstream & _stream, const char * _str )
//	{
//		size_t size = strlen(_str);
//		s_writeStream( _stream, size );
//		s_writeStreamCount( _stream, _str, size );
//
//		return true;
//	}
//
//	static bool s_writeStringW( std::ofstream & _stream, const char * _str )
//	{
//		//int size = ::MultiByteToWideChar( CP_UTF8, 0, _str, -1, 0, 0 );
//		size_t size = strlen(_str);
//		
//		s_writeStream( _stream, size );
//		s_writeStreamCount( _stream, _str, size );
//		//static WString s_buffer;
//		
//		//wchar_t * buffer = new wchar_t[size];
//		//::MultiByteToWideChar( CP_UTF8, 0, _str, -1, buffer, size );
//
//		////size_t size = wcslen(_str);
//		//s_writeStream( _stream, buffer, size );
//
//		//delete [] buffer;
//
//		return true;
//	}
//
//	static bool s_write_wchar_t( std::ofstream & _stream, const char * _str )
//	{
//		wchar_t wch;
//		::MultiByteToWideChar( CP_UTF8, 0, _str, -1, &wch, 1 );
//
//		s_writeStream( _stream, wch );
//		//static WString s_buffer;
//
//		//wchar_t * buffer = new wchar_t[size];
//		//::MultiByteToWideChar( CP_UTF8, 0, _str, -1, buffer, size );
//
//		////size_t size = wcslen(_str);
//		//s_writeStream( _stream, buffer, size );
//
//		//delete [] buffer;
//
//		return true;
//	}
//};
////////////////////////////////////////////////////////////////////////////
//bool XmlToBin::writeBinary( const wchar_t * _source, const wchar_t * _bin )
//{
//	m_serialization["Menge::String"] = &s_writeString;
//	m_serialization["Menge::WString"] = &s_writeStringW;
//	m_serialization["Menge::ConstString"] = &s_writeString;
//	m_serialization["bool"] = &s_writeBool;
//	m_serialization["unsigned short"] = &s_writeUnsignedShort;
//	m_serialization["int"] = &s_writeInt;
//	m_serialization["float"] = &s_writeFloat;
//	m_serialization["size_t"] = &s_writeSizet;
//	m_serialization["wchar_t"] = &s_write_wchar_t;
//	m_serialization["Menge::Resolution"] = &s_writeSizet2;
//	m_serialization["Menge::ColourValue"] = &s_writeFloat4;
//	m_serialization["Menge::Viewport"] = &s_writeFloat4;
//	m_serialization["mt::vec2f"] = &s_writeFloat2;
//	m_serialization["mt::vec3f"] = &s_writeFloat3;
//	m_serialization["mt::vec4f"] = &s_writeFloat4;
//	m_serialization["mt::mat3f"] = &s_writeFloat6;
//	m_serialization["std::vector<uint16>"] = &s_writeVector;
//
//	m_setSkipNodesAttributes["File"]["NoAtlas"] = 1;
//	m_setSkipNodesAttributes["File"]["NoJPEG"] = 1;
//	m_setSkipNodesAttributes["Text"]["MaxSize"] = 1;
//
//	//TiXmlDocument doc;
//	pugi::xml_document doc;
//
//	pugi::xml_parse_result result = doc.load_file( _source );
//
//	if( result == false )
//	{
//		m_error = result.description();
//
//		return false;
//	}
//
//	//TiXmlNode * protocol = doc.FirstChild();
//
//	std::ofstream fs( _bin, std::ios_base::binary );
//
//	char magic_number = 42; 
//	s_writeStream( fs, magic_number );
//	//s_writeStream( fs, _version );
//
//	//TiXmlNode * node = doc.FirstChild();
//
//	pugi::xml_node root = doc.document_element();
//
//	bool root_empty = root.empty();
//	s_writeStream( fs, root_empty );
//
//	if( root.empty() == true )
//	{
//		return true;
//	}
//
//	bool done = true;
//
//	if( this->writeNodeBinary_( fs, root ) == false )
//	{
//		done = false;	 
//	}
//	
//	//for( TiXmlNode * node = doc.FirstChild(); node; node = doc.IterateChildren( node ) )
//	//for( pugi::xml_node::iterator
//	//	it = root.begin(),
//	//	it_end = root.end();
//	//it != it_end;
//	//++it )
//	//{
//	//	const pugi::xml_node & node = *it;
//
//	//	//TiXmlElement * element = node->ToElement();
//
//	//	if( node.empty() == true )
//	//	{
//	//		continue;
//	//	}
//
//	//	//if( element == 0 )
//	//	//{
//	//	//	continue;
//	//	//}
//
//	//	if( this->writeNodeBinary_( fs, node ) == false )
//	//	{
//	//		done = false;
//	//		break;			 
//	//	}
//	//}
//
//	fs.close();
//
//	if( done == false )
//	{
//		_wremove( _bin );
//
//		return false;
//	}
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//const std::string & XmlToBin::getLastError() const
//{
//	return m_error;
//}
////////////////////////////////////////////////////////////////////////////
//bool XmlToBin::writeNodeBinary_( std::ofstream & _stream, const pugi::xml_node & _node )
//{
//	const char * nodeName = _node.name();
//
//	TMapNodes::const_iterator it_found = m_nodes.find( nodeName );
//
//	if( it_found == m_nodes.end() )
//	{
//		std::stringstream ss;
//		ss << "XmlToBin write not found node '" << nodeName << "'";
//		m_error = ss.str();
//
//		return false;
//	}
//
//	size_t id = it_found->second.id;
//	s_writeStream( _stream, id );
//
//	size_t sizeAttr = 0;
//
//	//for( TiXmlAttribute * attr = _element->FirstAttribute(); attr; attr = attr->Next() )
//	
//	for( pugi::xml_node::attribute_iterator
//		it = _node.attributes_begin(),
//		it_end = _node.attributes_end();
//	it != it_end;
//	++it )
//	{
//		const pugi::xml_attribute & attr = *it;
//
//		const char * attrName = attr.name();
//
//		TMapAttributes::const_iterator it_attr_found = it_found->second.attr.find( attrName );
//
//		if( it_attr_found == it_found->second.attr.end() )
//		{
//			int skip = m_setSkipNodesAttributes[nodeName][attrName];
//
//			if( skip == 1 )
//			{
//				continue;
//			}
//
//			std::stringstream ss;
//			ss << "XmlToBin write not found attr '" << attrName << "' in node '" << nodeName << "'";
//			m_error = ss.str();
//
//			return false;
//		}
//
//		++sizeAttr;
//	}
//
//	s_writeStream( _stream, sizeAttr );
//
//	//for( TiXmlAttribute * attr = _element->FirstAttribute(); attr; attr = attr->Next() )
//
//	for( pugi::xml_node::attribute_iterator
//		it = _node.attributes_begin(),
//		it_end = _node.attributes_end();
//	it != it_end;
//	++it )
//	{
//		const pugi::xml_attribute & attr = *it;
//
//		const char * attrName = attr.name();
//
//		TMapAttributes::const_iterator it_attr_found = it_found->second.attr.find( attrName );
//
//		if( it_attr_found == it_found->second.attr.end() )
//		{
//			int skip = m_setSkipNodesAttributes[nodeName][attrName];
//
//			if( skip == 1 )
//			{
//				continue;
//			}
//
//			std::stringstream ss;
//			ss << "XmlToBin write not found attr '" << attrName << "' in node '" << nodeName << "'";
//			m_error = ss.str();
//
//			return false;
//		}
//
//		size_t id = it_attr_found->second.id;
//
//		s_writeStream( _stream, id );
//
//		const std::string & type = it_attr_found->second.type;
//
//		TMapSerialization::iterator it_found = m_serialization.find(type);
//
//		if( it_found == m_serialization.end() )
//		{
//			std::stringstream ss;
//			ss << "XmlToBin write not found serializator '" << type << " for attr " << attrName << "' in node '" << nodeName << "'";
//			m_error = ss.str();
//
//			return false;
//		}
//
//		const char * attrValue = attr.value();
//
//		if( (*it_found->second)( _stream, attrValue ) == false )
//		{
//			std::stringstream ss;
//			ss << "XmlToBin write invalid serializator '" << type << " for attr " << attrName << "' in node '" << nodeName << "'";
//			m_error = ss.str();
//
//			return false;
//		}
//	}
//
//	size_t sizeNode = 0;
//
//	//for( TiXmlNode * node = _element->FirstChild(); node; node = _element->IterateChildren( node ) )
//
//	for( pugi::xml_node::iterator
//		it = _node.begin(),
//		it_end = _node.end();
//	it != it_end;
//	++it )
//	{
//		const pugi::xml_node & element = *it;
//
//		//TiXmlElement * element = node->ToElement();
//
//		if( element.empty() == true )
//		{
//			continue;
//		}
//
//		++sizeNode;
//	}
//
//	s_writeStream( _stream, sizeNode );
//
//	//for( TiXmlNode * node = _element->FirstChild(); node; node = _element->IterateChildren( node ) )
//
//	for( pugi::xml_node::iterator
//		it = _node.begin(),
//		it_end = _node.end();
//	it != it_end;
//	++it )
//	{
//		const pugi::xml_node & element = *it;
//
//		//TiXmlElement * element = node->ToElement();
//
//		if( element.empty() == true )
//		{
//			continue;
//		}
//
//		if( this->writeNodeBinary_( _stream, element ) == false )
//		{
//			return false;
//		}
//	}
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//extern "C" bool writeHeader( const wchar_t * _protocol, const wchar_t * _header )
//{
//	XmlToBin x2b;
//
//	if( x2b.readProtocol( _protocol ) == false )
//	{
//		return false;
//	}
//
//	x2b.writeHeader( _header );
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//extern "C" bool writeBinary( const wchar_t * _protocol, const wchar_t * _source, const wchar_t * _bin, char * _error )
//{
//	XmlToBin x2b;
//
//	if( x2b.readProtocol( _protocol ) == false )
//	{
//		const std::string & error = x2b.getLastError();
//		strcpy_s( _error, 255, error.c_str() );
//
//		return false;
//	}
//
//	if( x2b.writeBinary( _source, _bin ) == false )
//	{
//		const std::string & error = x2b.getLastError();
//		strcpy_s( _error, 255, error.c_str() );
//
//		return false;
//	}
//
//	return true;
//}