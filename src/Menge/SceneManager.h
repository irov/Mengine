#	pragma once

#	include <string>

class XmlElement;

namespace Menge
{
	class Node;	

	struct SceneManager
	{
		static Node * createNode( const std::string &_type );
		
		template<class T>
		static T* createNodeT( const std::string &_type )
		{
			return dynamic_cast<T*>(createNode( _type ));
		}

		static Node * createNodeFromXml( const std::string &_file);
		static Node * createNodeFromXmlData( const std::string & _xml_data );

		static void loaderNode_( XmlElement * _xml );

		template<class T>
		static T * createNodeFromXmlT( const std::string &_file)
		{
			return dynamic_cast<T*>(createNodeFromXml(_file));
		}
		static bool loadNode(Node *_node, const std::string &_xml);
	};
}