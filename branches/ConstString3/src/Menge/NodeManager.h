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
		Node * createNode( const ConstString& _type );
		
		template<class T>
		T * createNodeT( const ConstString& _type )
		{
			Node * node = this->createNode( _type );

			return static_cast<T*>(node);
		}
	};
}
