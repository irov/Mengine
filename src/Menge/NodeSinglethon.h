#	pragma once

#	include "Node.h"

#	include <list>

namespace Menge
{
	class NodeSinglethon
		: public virtual Node
	{
	public: 
	/*	void setParent( Node * _parent ) override;

		bool addChildren( Node * _node ) override;
		Node * getChildren( const std::string & _name, bool _recursion ) override;
		void removeChildren( Node *_node ) override;
		bool isChildren( Node *_node ) override;
		Node* getParent() override;
*/
	public:
		void foreachChildren( NodeForeach & _foreach );
	};
}