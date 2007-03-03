#	pragma once

#	define TIXML_USE_STL

#	include "tinyxml/tinyxml.h"

#	include "Utility/string_algorithm.h"
#	include "math/mat4.h"

/****************************************************************************
XML_TYPE_NODE

��� ����(������)
****************************************************************************/
#	define XML_TYPE_NODE\
	TiXmlElement *

/****************************************************************************
XML_CURRENT_NODE

������� ����(������), ������� ������ ��������� 

�.�. ������������� ��� �������� � ������ ����� �����. ����� ��� XML_TYPE_NODE
****************************************************************************/
#	define XML_CURRENT_NODE\
	irov_xml_current_tree

/****************************************************************************
XML_PARSE_DATA [XML_DATA]

������� ����, � ������� ���� ���������� ���������. [new 27.04]
****************************************************************************/
#	define XML_PARSE_DATA( XML_DATA )\
	for( bool irov_xml_parse_once = true ; irov_xml_parse_once == true; )\
	for( \
	TiXmlDocument irov_xml_document;\
	irov_xml_parse_once==true;\
	)\
	if( (irov_xml_parse_once = irov_xml_document.LoadData(XML_DATA)) == true )\
	for(\
	TiXmlElement * irov_xml_current_tree = irov_xml_document.FirstChildElement();\
	irov_xml_current_tree != 0;\
	irov_xml_current_tree = irov_xml_current_tree->NextSiblingElement()\
	)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == true;irov_xml_parse_once = false)\

/****************************************************************************
XML_PARSE_FILE [XML_NAME]

������� ����, � ������� ���� ��������� ���������. 
���� ���� �� ������ �� ��� ������ ��� ���� �����.
****************************************************************************/
#	define XML_PARSE_FILE( XML_NAME )\
	for( bool irov_xml_parse_once = true ; irov_xml_parse_once == true; )\
	for( \
	TiXmlDocument irov_xml_document;\
	irov_xml_parse_once==true;\
	)\
	if( (irov_xml_parse_once = irov_xml_document.LoadFile(XML_NAME)) == true )\
	for(\
	TiXmlElement * irov_xml_current_tree = irov_xml_document.FirstChildElement();\
	irov_xml_current_tree != 0;\
	irov_xml_current_tree = irov_xml_current_tree->NextSiblingElement()\
	)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == true;irov_xml_parse_once = false)\

/*****************************************************************************
XML_INVALID_PARSE

���������� �������� ����.
*****************************************************************************/
#	define	XML_INVALID_PARSE() else 

#	define	XML_CONTINUE() continue

/*****************************************************************************
XML_FOR_EACH

���������� �������� ����.
*****************************************************************************/
#	define XML_FOR_EACH()\
	for(\
	TiXmlElement *\
	irov_xml_it_b = irov_xml_current_tree->FirstChildElement();\
	irov_xml_it_b != 0;\
	irov_xml_it_b = irov_xml_it_b->NextSiblingElement()\
	)\
	for( bool irov_xml_parse_once = false; irov_xml_parse_once == false;)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == false;)\
	for( TiXmlElement * irov_xml_current_tree = irov_xml_it_b;\
	irov_xml_parse_once == false;\
	irov_xml_parse_once = true \
	)

/****************************************************************************
XML_FOR_EACH_TREE [ XML_TREE ]

���������� ���� � ���������� ��� ������.
****************************************************************************/
#	define XML_FOR_EACH_TREE( XML_TREE )\
	for(\
	TiXmlElement *\
	irov_xml_it_b = (XML_TREE)->FirstChildElement();\
	irov_xml_it_b != 0;\
	irov_xml_it_b = irov_xml_it_b->NextSiblingElement()\
	)\
	for( bool irov_xml_parse_once = false; irov_xml_parse_once == false;)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == false;)\
	for( TiXmlElement * irov_xml_current_tree = irov_xml_it_b;\
	irov_xml_parse_once == false;\
	irov_xml_parse_once = true, irov_xml_else_if = false\
	)

/****************************************************************************
XML_FOR_EACH_ATTRIBUTES [ XML_TREE ]

���������� �������� � ���������� ��� ������.
****************************************************************************/
#	define XML_FOR_EACH_ATTRIBUTES( XML_TREE )\
	for( bool irov_xml_parse_once = false; irov_xml_parse_once == false;)\
	for(  TiXmlElement * irov_xml_current_tree = XML_TREE;\
	irov_xml_parse_once == false;\
	irov_xml_parse_once = true \
	)

#	define XML_FOR_EACH_FORCE( XML_TREE )\
	for( bool irov_xml_parse_once = false; irov_xml_parse_once == false;)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == false;)\
	for(  TiXmlElement * irov_xml_current_tree = XML_TREE;\
	irov_xml_parse_once == false;\
	irov_xml_parse_once = true, irov_xml_else_if = false\
	)

/****************************************************************************
XML_TITLE_NODE

���������� ��� ����.
****************************************************************************/

#	define XML_TITLE_NODE_CHARP\
	(irov_xml_current_tree->Value())

#	define XML_TITLE_NODE\
	(Utility::string_from_char(XML_TITLE_NODE_CHARP))

/****************************************************************************
XML_VALUE_NODE []

���������� �������� ����.
****************************************************************************/
#	define XML_VALUE_NODE()\
	((irov_xml_current_tree->FirstChild())?\
	irov_xml_current_tree->FirstChild()->ValueStr():\
	Utility::string_empty());

/****************************************************************************
XML_CHECK_NODE [ TITLE ]

��������� �� ����� ������� ����������� ����, ���� ��� ���� ��������� � TITLE
�� ������ ������� ������.
****************************************************************************/
#	define XML_CHECK_NODE( TITLE )\
	if( irov_xml_else_if == true )continue;else\
	if( XML_TITLE_NODE == TITLE )\
	for(;irov_xml_else_if == false;irov_xml_else_if=true)
 
/****************************************************************************
XML_CHECK_NODE_FOR_EACH [ TITLE ]

��������� �� ����� ������� ����������� ����, ���� ��� ���� ��������� � TITLE
�� ������ ������� ������.
****************************************************************************/
#	define XML_CHECK_NODE_FOR_EACH( TITLE )\
	if( irov_xml_else_if == true )continue;else\
	if( XML_TITLE_NODE == TITLE )\
	for(;irov_xml_else_if == false;irov_xml_else_if=true)\
	XML_FOR_EACH()

/****************************************************************************
XML_ATTRIBUTES_NODE [ NAME ]

���������� �������� �������� ����� std::string, ���� ������ � ������� ���� 
������� NAME
****************************************************************************/

#	define XML_ATTRIBUTES_NODE_CHARP( NAME ) \
	(irov_xml_current_tree->Attribute( NAME ))

#	define XML_ATTRIBUTES_NODE( NAME ) \
	(xml::parse::get_string_ref_attribute(irov_xml_current_tree, NAME ))

/****************************************************************************
XML_CHECK_ATTRIBUTE [ SET | NAME ]

HACK
****************************************************************************/
#	define XML_CHECK_ATTRIBUTE( ATTRIBUTE, SET , VALUE  )\
	if( strcmp(XML_ATTRIBUTES_NODE_CHARP( ATTRIBUTE ),#VALUE) == 0 )\
	{\
	SET = VALUE;\
	}

/****************************************************************************
XML_DEF_ATTRIBUTES_NODE [ NAME ]

���������� ��������� NAME � ������� ���������� �������� 
�������� NAME � ������� ����.
****************************************************************************/
#	define XML_DEF_ATTRIBUTES_NODE( NAME ) \
	const std::string & NAME = XML_ATTRIBUTES_NODE( #NAME );

#	define XML_VALUE_ATTRIBUTE( NAME, VALUE )\
	xml::parse::GetValueAttribute( (VALUE) , (NAME) , XML_CURRENT_NODE )

#	define XML_VALUE_ATTRIBUTE_DEFAULT( NAME, VALUE , DEFAULT )\
	if( XML_ATTRIBUTES_NODE(NAME).empty() == false )\
	{\
	xml::parse::GetValueAttribute( (VALUE) , (NAME) , XML_CURRENT_NODE );\
	}\
	else\
	{\
	VALUE = DEFAULT;\
	}

#	define XML_CHECK_VALUE_NODE( TITLE , ATTRIBUTE, VAR )\
	if( XML_TITLE_NODE == (TITLE) )\
{\
	XML_VALUE_ATTRIBUTE((ATTRIBUTE),(VAR));\
}

#	define XML_CHECK_VALUE_NODE_BOOL( TITLE , ATTRIBUTE, VAR )\
	XML_CHECK_NODE( TITLE )\
{\
	std::string Attribute;\
	XML_VALUE_ATTRIBUTE((ATTRIBUTE),(Attribute));\
	Utility::string_toupper(Attribute);\
	VAR = ( Attribute == "TRUE" )?true:false;\
}

namespace xml
{
	namespace parse
	{
		inline const std::string & get_string_ref_attribute( TiXmlElement *_xml, const std::string & _attr )
		{
			const std::string *value = _xml->Attribute(_attr);

			if( value == 0 )
			{
				static std::string Empty;
				return Empty;
			}

			return *value;
		}

		inline const std::string & get_string_ref_attribute( TiXmlElement *_xml, const char * _attr )
		{			
			if( _attr == 0 )
			{
				static std::string Empty;
				return Empty;
			}

			return get_string_ref_attribute( _xml, std::string(_attr) );
		}

		template<class T>
		inline bool GetValueAttributeSimple(T &Value, const std::string &Name, const char *Type, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf(VALUE.c_str(),Type,&Value);

			return true;
		}

		template<class T>
		inline bool GetValueAttribute(T &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			char strError[1024];

			sprintf(strError,"Error parse xml: Invalid Attribute '%s'"
				, Name.c_str()
				);

			throw std::exception(strError);

			return false;
		}

#	define XMLP_SIMPLE_PARSE( K )\
	return GetValueAttributeSimple(Value,Name, (K) ,irov_xml_current_tree)

		template<>
		inline bool GetValueAttribute(bool &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			unsigned int Bool;
			sscanf(VALUE.c_str(),"%u",&Bool);

			Value = (Bool == 0)?false:true;

			return true;			
		}

		template<>
		inline bool GetValueAttribute(unsigned int &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			XMLP_SIMPLE_PARSE("%u");
		}

		template<>
		inline bool GetValueAttribute(int &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			XMLP_SIMPLE_PARSE("%d");
		}

		template<>
		inline bool GetValueAttribute(float &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			XMLP_SIMPLE_PARSE("%f");
		}

		template<>
		inline bool GetValueAttribute(std::string &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			Value = VALUE;

			return true;
		}


		template<>
		inline bool GetValueAttribute<mt::vec2f>(mt::vec2f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf(VALUE.c_str(),"%f;%f"
				,&Value.x,&Value.y
				);

			return true;
		}

		template<>
		inline bool GetValueAttribute<mt::vec3f>(mt::vec3f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf(VALUE.c_str(),"%f;%f;%f"
				,&Value.x,&Value.y,&Value.z
				);

			return true;
		}

		template<>
		inline bool GetValueAttribute<mt::vec4f>(mt::vec4f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf(VALUE.c_str(),"%f;%f;%f;%f"
				,&Value.x,&Value.y,&Value.z,&Value.w
				);

			return true;
		}

		template<>
		inline bool GetValueAttribute<mt::mat3f>(mt::mat3f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf(VALUE.c_str(),"%f;%f;%f;%f;%f;%f"
				,&Value[0][0],&Value[0][1]
				,&Value[1][0],&Value[1][1]
				,&Value[2][0],&Value[2][1]
			);

			Value[0][2] = 0.f;
			Value[1][2] = 0.f;
			Value[2][2] = 1.f;				//��� ������ ���� 1

			return true;
		}

		template<>
		inline bool GetValueAttribute<mt::mat4f>(mt::mat4f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			const std::string &VALUE = XML_ATTRIBUTES_NODE(Name);

			if( VALUE.empty() == true )
			{
				return false;
			}

			sscanf(VALUE.c_str(),"%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f"
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