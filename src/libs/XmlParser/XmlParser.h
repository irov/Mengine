#	pragma once

#	include <string>

#	include "math/mat4.h"

class TiXmlDocument;
class TiXmlElement;

TiXmlDocument * TiXmlDocumentLoadFile( const std::string & _file );
TiXmlDocument * TiXmlDocumentLoadData( const std::string & _data );
void TiXmlDocumentRelese( TiXmlDocument * _document );
TiXmlElement * TiXmlDocumentFirstChildElement( TiXmlDocument * _document );
TiXmlElement * TiXmlElementFirstChildElement( TiXmlElement * _element );
TiXmlElement * TiXmlElementNextSiblingElement( TiXmlElement * _element );
const std::string & TiXmlElementValue( TiXmlElement * _element );
const std::string & TiXmlElementTitle( TiXmlElement * _element );
const std::string & TiXmlElementAttribute( TiXmlElement * _element, const std::string & _name );


/****************************************************************************
XML_TYPE_NODE

тип ноды(дерева)
****************************************************************************/
#	define XML_TYPE_NODE\
	TiXmlElement *

/****************************************************************************
XML_CURRENT_NODE

текущея нода(дерево), которая сейчас парситься 

З.Ы. Используеться для передачи в другие места парса. имеет тип XML_TYPE_NODE
****************************************************************************/
#	define XML_CURRENT_NODE\
	irov_xml_current_tree

/****************************************************************************
XML_PARSE_FILE [XML_NAME]

Парсить фаил, и перебор всех внутриних елементов. 
если файл не найден то Хмл объект сам себя убьет.
****************************************************************************/
#	define XML_PARSE_FILE( XML_NAME )\
	for( bool irov_xml_parse_once = true ; irov_xml_parse_once == true; )\
	for( \
	TiXmlDocument * irov_xml_document = 0;\
	irov_xml_parse_once==true;\
	TiXmlDocumentRelese( irov_xml_document )\
	)\
	if( irov_xml_parse_once = ((irov_xml_document = TiXmlDocumentLoadFile( XML_NAME )) != 0) )\
	for(\
	TiXmlElement * irov_xml_current_tree = TiXmlDocumentFirstChildElement( irov_xml_document );\
	irov_xml_current_tree != 0;\
	irov_xml_current_tree = TiXmlElementNextSiblingElement(irov_xml_current_tree)\
	)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == true;irov_xml_parse_once = false)

#define XML_FOR_EACH_DOCUMENT( XML_DOCUMENT )\
	for( bool irov_xml_parse_once = true ; irov_xml_parse_once == true; )\
	for( \
	TiXmlDocument * irov_xml_document = XML_DOCUMENT;\
	irov_xml_parse_once==true && irov_xml_document;\
	TiXmlDocumentRelese( irov_xml_document )\
	)\
	if( irov_xml_parse_once = true )\
	for(\
	TiXmlElement * irov_xml_current_tree = TiXmlDocumentFirstChildElement(irov_xml_document);\
	irov_xml_current_tree != 0;\
	irov_xml_current_tree = TiXmlElementNextSiblingElement(irov_xml_current_tree)\
	)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == true;irov_xml_parse_once = false)

/*****************************************************************************
XML_INVALID_PARSE

Перебирает вложеные теги.
*****************************************************************************/
#	define	XML_INVALID_PARSE() else 

#	define	XML_CONTINUE() continue

/*****************************************************************************
XML_FOR_EACH

Перебирает вложеные теги.
*****************************************************************************/
#	define XML_FOR_EACH()\
	for(\
	TiXmlElement *\
	irov_xml_it_b = TiXmlElementFirstChildElement(irov_xml_current_tree);\
	irov_xml_it_b != 0;\
	irov_xml_it_b = TiXmlElementNextSiblingElement(irov_xml_it_b)\
	)\
	for( bool irov_xml_parse_once = false; irov_xml_parse_once == false;)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == false;)\
	for( TiXmlElement * irov_xml_current_tree = irov_xml_it_b;\
	irov_xml_parse_once == false;\
	irov_xml_parse_once = true \
	)

/****************************************************************************
XML_FOR_EACH_TREE [ XML_TREE ]

перебирает теги у полученого Хмл дерево.
****************************************************************************/
#	define XML_FOR_EACH_TREE( XML_TREE )\
	for(\
	TiXmlElement *\
	irov_xml_it_b = TiXmlElementFirstChildElement( XML_TREE );\
	irov_xml_it_b != 0;\
	irov_xml_it_b = TiXmlElementNextSiblingElement(irov_xml_it_b)\
	)\
	for( bool irov_xml_parse_once = false; irov_xml_parse_once == false;)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == false;)\
	for( TiXmlElement * irov_xml_current_tree = irov_xml_it_b;\
	irov_xml_parse_once == false;\
	irov_xml_parse_once = true, irov_xml_else_if = false\
	)

/****************************************************************************
XML_FOR_EACH_ATTRIBUTES [ XML_TREE ]

перебирает атрибуты у полученого Хмл дерево.
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

возвращает имя тега.
****************************************************************************/

#	define XML_TITLE_NODE\
	TiXmlElementTitle( XML_CURRENT_NODE )

/****************************************************************************
XML_VALUE_NODE []

возвращает значение тега.
****************************************************************************/
#	define XML_VALUE_NODE()\
	TiXmlElementValue( XML_CURRENT_NODE )

/****************************************************************************
XML_CHECK_NODE [ TITLE ]

проверяет во время парсера встречающии теги, если имя тега совпадает с TITLE
то парсер заходит внутрь.
****************************************************************************/
#	define XML_CHECK_NODE( TITLE )\
	if( irov_xml_else_if == true )continue;else\
	if( XML_TITLE_NODE == TITLE )\
	for(;irov_xml_else_if == false;irov_xml_else_if=true)

/****************************************************************************
XML_CHECK_NODE_FOR_EACH [ TITLE ]

проверяет во время парсера встречающии теги, если имя тега совпадает с TITLE
то парсер заходит внутрь.
****************************************************************************/
#	define XML_CHECK_NODE_FOR_EACH( TITLE )\
	if( irov_xml_else_if == true )continue;else\
	if( XML_TITLE_NODE == TITLE )\
	for(;irov_xml_else_if == false;irov_xml_else_if=true)\
	XML_FOR_EACH()

/****************************************************************************
XML_ATTRIBUTES_NODE [ NAME ]

возвращает значение атрибута типом std::string, если найден в текущем теге 
атрибут NAME
****************************************************************************/

#	define XML_ATTRIBUTES_NODE( NAME ) \
	TiXmlElementAttribute( irov_xml_current_tree, NAME )

/****************************************************************************
XML_DEF_ATTRIBUTES_NODE [ NAME ]

создаеться переменая NAME в которою заноситься значение 
атрибута NAME в текущем теге.
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

namespace xml
{
	namespace parse
	{
		template<class T>
		inline bool GetValueAttribute(T &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree)
		{
			char strError[1024];

			sprintf(strError,"Error parse xml: Invalid Attribute '%s'"
				, Name.c_str()
				);

			throw std::exception( strError );

			return false;
		}

		bool GetValueAttribute(bool &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(unsigned int &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(int &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(float &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(std::string &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(mt::vec2f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(mt::vec3f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(mt::vec4f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(mt::mat3f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
		bool GetValueAttribute(mt::mat4f &Value, const std::string &Name, XML_TYPE_NODE irov_xml_current_tree);
	}
}