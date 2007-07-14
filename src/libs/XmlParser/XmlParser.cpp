#	include "XmlParser.h"

#	define TIXML_USE_STL
#	include "tinyxml/tinyxml.h"

//////////////////////////////////////////////////////////////////////////
static const std::string & string_empty()
{
	static std::string s_empty;
	return s_empty;
}
//////////////////////////////////////////////////////////////////////////
TiXmlDocument * TiXmlDocumentLoadFile( const std::string & _file )
{
	TiXmlDocument * document = new TiXmlDocument;

	if( document->LoadFile( _file ) == false )
	{
		delete document;

		return 0;
	}
	
	return document;	
}
//////////////////////////////////////////////////////////////////////////
TiXmlDocument * TiXmlDocumentLoadData( const std::string & _data )
{
	TiXmlDocument * document = new TiXmlDocument;

	document->Parse( _data.c_str(), 0, TIXML_DEFAULT_ENCODING );

	if( document->Error() == true )
	{
		delete document;
		return false;
	}

	return document;
}
//////////////////////////////////////////////////////////////////////////
void TiXmlDocumentRelese( TiXmlDocument * _document )
{
	delete _document;
}
//////////////////////////////////////////////////////////////////////////
TiXmlElement * TiXmlDocumentFirstChildElement( TiXmlDocument * _document )
{
	return _document->FirstChildElement();
}
//////////////////////////////////////////////////////////////////////////
TiXmlElement * TiXmlElementFirstChildElement( TiXmlElement * _element )
{
	return _element->FirstChildElement();
}
//////////////////////////////////////////////////////////////////////////
TiXmlElement * TiXmlElementNextSiblingElement( TiXmlElement * _element )
{
	return _element->NextSiblingElement();
}
//////////////////////////////////////////////////////////////////////////
const std::string & TiXmlElementValue( TiXmlElement * _element )
{
	TiXmlNode * node = _element->FirstChild();

	if( node )
	{
		return node->ValueStr();
	}

	return string_empty();
}
//////////////////////////////////////////////////////////////////////////
const std::string & TiXmlElementTitle( TiXmlElement * _element )
{
	return _element->ValueStr();
}
//////////////////////////////////////////////////////////////////////////
const std::string & TiXmlElementAttribute( TiXmlElement * _element, const std::string & _name )
{
	const std::string * attr = _element->Attribute( _name );

	if( attr )
	{
		return *attr;
	}

	return string_empty();
}
//////////////////////////////////////////////////////////////////////////
TiXmlElement * TiXmlElementCreate( const std::string & _name )
{
	return new TiXmlElement( _name );
}
//////////////////////////////////////////////////////////////////////////
void TiXmlElementSetAttribute( TiXmlElement * _element, const std::string & _attribute, const std::string & _value )
{
	_element->SetAttribute( _attribute, _value );
}
//////////////////////////////////////////////////////////////////////////
void TiXmlElementInsertEndChild( TiXmlElement * _element, TiXmlElement * _children )
{
	_element->InsertEndChild( *_children );
}
//////////////////////////////////////////////////////////////////////////
void TiXmlElementRemove( TiXmlElement * _element )
{
	delete _element;
}

#	define XMLP_SIMPLE_PARSE( K )\
	return GetValueAttributeSimple(Value,Name, (K) ,irov_xml_current_tree)

namespace xml
{
	namespace parse
	{
		template<class T>
		bool GetValueAttributeSimple( 
			T &Value, 
			const std::string &Name, 
			const char *Type, 
			XML_TYPE_NODE irov_xml_current_tree )
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf_s( VALUE.c_str(), Type, &Value);

			return true;
		}

		bool GetValueAttribute(bool &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			unsigned int Bool;
			sscanf_s(VALUE.c_str(),"%u",&Bool);

			Value = (Bool == 0)?false:true;

			return true;			
		}

		bool GetValueAttribute(unsigned int &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			XMLP_SIMPLE_PARSE("%u");
		}

		bool GetValueAttribute(int &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			XMLP_SIMPLE_PARSE("%d");
		}

		bool GetValueAttribute(float &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			XMLP_SIMPLE_PARSE("%f");
		}

		bool GetValueAttribute(std::string &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			Value = VALUE;

			return true;
		}


		bool GetValueAttribute(mt::vec2f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf_s(VALUE.c_str(),"%f;%f"
				,&Value.x,&Value.y
				);

			return true;
		}

		bool GetValueAttribute(mt::vec3f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf_s(VALUE.c_str(),"%f;%f;%f"
				,&Value.x,&Value.y,&Value.z
				);

			return true;
		}

		bool GetValueAttribute(mt::vec4f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf_s(VALUE.c_str(),"%f;%f;%f;%f"
				,&Value.x,&Value.y,&Value.z,&Value.w
				);

			return true;
		}

		bool GetValueAttribute(mt::mat3f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf_s(VALUE.c_str(),"%f;%f;%f;%f;%f;%f"
				,&Value[0][0],&Value[0][1]
			,&Value[1][0],&Value[1][1]
			,&Value[2][0],&Value[2][1]
			);

			Value[0][2] = 0.f;
			Value[1][2] = 0.f;
			Value[2][2] = 1.f;			

			return true;
		}

		bool GetValueAttribute(mt::mat4f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf_s(VALUE.c_str(),"%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f"
				,&Value[0][0],&Value[0][1],&Value[0][2]
			,&Value[1][0],&Value[1][1],&Value[1][2]
			,&Value[2][0],&Value[2][1],&Value[2][2]
			,&Value[3][0],&Value[3][1],&Value[3][2]
			);

			Value[0][3] = 0.f;
			Value[1][3] = 0.f;
			Value[2][3] = 0.f;
			Value[3][3] = 1.f;

			return true;
		}
	}
}