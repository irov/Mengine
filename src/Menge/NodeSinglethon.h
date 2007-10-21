#	pragma once

#	include "NodeCore.h"

#	include <list>

namespace Menge
{
	class NodeSinglethon
		: public NodeCore
	{
	public: 
		void setParent( Node * _parent ) override;

		bool addChildren( Node * _node ) override;
		Node * getChildren( const std::string & _name, bool _recursion ) override;
		void removeChildren( Node *_node ) override;
		bool isChildren( Node *_node ) override;

	public:
		void foreachChildren( const NodeForeach & _foreach ) override;
	};
}