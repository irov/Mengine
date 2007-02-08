#	pragma once 

#	include "Visitor.h"

namespace Menge
{
	template<class T>
	class VisitorMask
		: public Visitor
	{
		void call_impl( Node *_base, ... )
		{
			Visitor::call( _base, _base );
		}
		
		void call_impl( Node *_base, T * _o  )
		{
			mask_call(_o);
		}
	
	public:
		void call( Node *_node, Node *_base){ call_impl( _base, _node  ); }
		void call( Allocator *_node, Node *_base){ call_impl( _base, _node  );	}
		void call( Renderable *_node, Node *_base){ call_impl( _base, _node  ); }
		void call( HotSpot *_node, Node *_base){ call_impl( _base, _node ); }
		void call( Sprite *_node, Node *_base){ call_impl( _base, _node ); }
		void call( Dialog *_node, Node *_base){ call_impl( _base, _node ); }

	protected:
		virtual void mask_call( T *_node) = 0;
	};
}