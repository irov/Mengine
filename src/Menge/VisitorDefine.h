#	pragma once

#	ifdef VISITOR_BASE
#	define VISITABLE_CLASS( X ) virtual void call( X *_node){}
#	elif VISITOR_MASK
#	define VISITABLE_CLASS( X ) void call( Node *_node) override{ call_impl(_node);	}
#	define VISITOR_MASK_DECLARE()\
	private:\
	template<typename O>\
	void call_impl( O * ){}\
	void call_impl( T * _node)\
	{\
		mask_call(_node);\
	}\
	public:
#	endif