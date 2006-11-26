#	pragma once

#	undef VISITABLE_CLASS
#	define VISITABLE_CLASS( X ) void call( X *_node) override{ call_impl(_node);	}

#	define VISITOR_MASK_DECLARE( CLASS )\
	private:\
	template<typename O>\
	void call_impl( O * _o )\
	{\
		_o->foreachChildren(this);\
	}\
	void call_impl( CLASS * _node)\
	{\
	mask_call(_node);\
	}\
	private: