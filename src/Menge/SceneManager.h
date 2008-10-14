#	pragma once

#	include "Config/Typedef.h"
//#	include <string>

class XmlElement;

namespace Menge
{
	class Node;	

	class SceneManager
	{
	public:
		static Node * createNode( const String& _type );
		
		template<class T>
		static T* createNodeT( const String& _type )
		{
			return dynamic_cast<T*>( createNode( _type ) );
		}

		static Node * createNodeFromXml( const String& _file );
		static Node * createNodeFromXmlData( const String& _xml_data );

		template<class T>
		static T * createNodeFromXmlT( const String& _file)
		{
			return dynamic_cast<T*>(createNodeFromXml(_file));
		}

		static bool loadNode(Node *_node, const String& _xml );
	};
}