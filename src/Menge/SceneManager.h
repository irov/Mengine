#	pragma once

#	include "Config/Typedef.h"

#	include "Holder.h"

#	include "Factory.h"

class XmlElement;

namespace Menge
{
	class Node;	

	class SceneManager
	{
	public:
		void registrationType( const String& _type, Factory::TGenFunc _func );

		Node * createNode( const String& _type );
		
		template<class T>
		T * createNodeT( const String& _type )
		{
			return dynamic_cast<T*>( createNode( _type ) );
		}

		Node * createNodeFromXml( const String& _file );
		Node * createNodeFromXmlData( const String& _xml_data );

		template<class T>
		T * createNodeFromXmlT( const String& _file)
		{
			return dynamic_cast<T*>(createNodeFromXml(_file));
		}

		bool loadNode(Node *_node, const String& _xml );

		void releaseNode( Node* _node );

	protected:
		Factory m_factory;
	};
}
