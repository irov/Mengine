#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "Manager/ConstManager.h"

#	include "Factory/FactoryManager.h"


namespace Menge
{
	class Node;	
	class NodeFactory;

	class NodeManager
		: public Holder<NodeManager>
		, public FactoryManager
	{
	public:
		NodeManager();

	public:
		Node * createNode( const ConstString& _name, const ConstString& _type );

	public:
		Node * createNode( const ConstString& _type );
		
		template<class T>
		T * createNodeT( const ConstString& _type )
		{
			return dynamic_cast<T*>( createNode( _type ) );
		}

		Node * createNodeFromXml( const ConstString& _pakName, const ConstString& _filename );
		Node * createNodeFromXmlData( const String& _xml_data );

		template<class T>
		T * createNodeFromXmlT( const ConstString& _file)
		{
			return dynamic_cast<T*>(createNodeFromXml(_file));
		}

	public:
		bool loadNode( Node *_node, const ConstString& _pakName, const ConstString& _filename );

	protected:
		typedef std::map<ConstString, NodeFactory *> TMapGenerator;
		TMapGenerator m_generator;
	};
}
