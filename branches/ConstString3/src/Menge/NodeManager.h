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
		Node * createNode( const ConstString& _name, const ConstString& _type, const ConstString& _tag );
		
		template<class T>
		T * createNodeT( const ConstString& _name, const ConstString& _type, const ConstString& _tag )
		{
			Node * node = this->createNode( _name, _type, _tag );

			return static_cast<T*>(node);
		}
	};
}
