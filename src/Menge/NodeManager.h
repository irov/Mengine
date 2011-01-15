#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "Factory/FactoryManager.h"

namespace Menge
{
	class Node;	
	class NodeFactory;
	class FactoryIdentity;

	class NodeManager
		: public Holder<NodeManager>
		, public FactoryManager
	{
	public:
		NodeManager( FactoryIdentity * _factoryIdentity );

	public:
		Node * createNode( const String& _type );
		
		template<class T>
		T * createNodeT( const String& _type )
		{
			return dynamic_cast<T*>( createNode( _type ) );
		}

		Node * createNodeFromXml( const String& _pakName, const String& _filename );
		Node * createNodeFromXmlData( const String& _xml_data );

		template<class T>
		T * createNodeFromXmlT( const String& _pakName, const String& _file )
		{
			return dynamic_cast<T*>(createNodeFromXml(_pakName, _file));
		}

	public:
		bool loadNode(Node *_node, const String& _pakName, const String& _filename );

	protected:
		typedef std::map<String, NodeFactory *> TMapGenerator;
		TMapGenerator m_generator;

		FactoryIdentity * m_factoryIdentity;
	};
}
