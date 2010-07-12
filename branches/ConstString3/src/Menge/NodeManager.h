#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "Core/ConstString.h"

#	include "FactoryManager.h"


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

		//Node * createNodeFromXml( const ConstString& _pakName, const String& _filename );
		Node * createNodeFromBinResource( const ConstString & _binResource );

		template<class T>
		T * createNodeFromXmlT( const ConstString& _file)
		{
			return dynamic_cast<T*>(createNodeFromXml(_file));
		}

	public:
		bool loadNode( Node *_node, const ConstString& _pakName, const String& _filename );

	protected:
		typedef std::map<ConstString, NodeFactory *> TMapGenerator;
		TMapGenerator m_generator;
	};
}
