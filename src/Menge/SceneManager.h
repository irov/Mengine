#	pragma once

#	include <string>

namespace Menge
{
	class Node;
	class Viewport;

	struct SceneManager
	{
		static Node * createNode( const std::string &_type );
		
		template<class T>
		static T* createNodeT( const std::string &_type )
		{
			return dynamic_cast<T*>(createNode( _type ));
		}

		static Node * createNodeFromXml( const std::string &_file);

		template<class T>
		static T * createNodeFromXmlT( const std::string &_file)
		{
			return dynamic_cast<T*>(createNodeFromXml(_file));
		}
		static bool loadNode(Node *_node, const std::string &_xml);
	};
}