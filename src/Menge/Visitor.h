#	pragma once


/*#	define VISITOR_TYPES\
	(Node)(Sprite)(Animation)\
	(Layer2D)(Track)(Arrow)\
	(HotSpot)(TextField)(SoundEmitter)\
	(Emitter)(Point)(RigidBody2D)\
	(TileMap)(Camera2D)(Entity)\
	(LayerScene)

#   define VISITOR_PP_RESULT(res) VISITOR_PP_RESULT_I(res)
#   define VISITOR_PP_RESULT_I(x) x

#	define VISITOR_PP_SEQ_PROC

#   define VISITOR_PP_SEQ_TAIL(Q) VISITOR_PP_RESULT(VISITOR_PP_SEQ_TAIL_I Q)
#   define VISITOR_PP_SEQ_TAIL_I(x)

#   define VISITOR_PP_SEQ_ENUM(Q) VISITOR_PP_SEQ_HEAD(Q) VISITOR_PP_SEQ_ENUM_I(VISITOR_PP_SEQ_TAIL(Q))
#   define VISITOR_PP_SEQ_ENUM_I(Q) VISITOR_PP_RESULT(VISITOR_PP_RESULT(VISITOR_PP_SEQ_FE_I1 Q) ## _END)

#	define VISITOR_PP_PARA_FIRST_PARAM(F) VISITOR_PP_PARA_FIRST_PARAM_I F
#	define VISITOR_PP_PARA_FIRST_PARAM_I(x,y) VISITOR_PP_SEQ_PROC( x )

#	define VISITOR_PP_SEQ_HEAD(Q) VISITOR_PP_RESULT(VISITOR_PP_PARA_FIRST_PARAM( (VISITOR_PP_SEQ_HEAD_I Q) ) ) 
#   define VISITOR_PP_SEQ_HEAD_I(Q) Q ,

#   define VISITOR_PP_SEQ_FE_I1_END
#   define VISITOR_PP_SEQ_FE_I2_END
#   define VISITOR_PP_SEQ_FE_I1(x) VISITOR_PP_SEQ_PROC( x ) VISITOR_PP_SEQ_FE_I2
#   define VISITOR_PP_SEQ_FE_I2(x) VISITOR_PP_SEQ_PROC( x ) VISITOR_PP_SEQ_FE_I1

#	undef VISITOR_PP_SEQ_PROC
#	define VISITOR_PP_SEQ_PROC( TYPE )\
	virtual void visit_impl( class TYPE * _node ) = 0;*/

namespace Menge
{

	class Visitor
	{
	public:
		//VISITOR_PP_SEQ_ENUM( VISITOR_TYPES )
		  virtual void visit_impl( class Node * _node ) = 0;
          virtual void visit_impl( class Sprite * _node ) = 0;
          virtual void visit_impl( class Animation * _node ) = 0;
          virtual void visit_impl( class Layer2D * _node ) = 0;
          virtual void visit_impl( class Track * _node ) = 0;
          virtual void visit_impl( class Arrow * _node ) = 0;
          virtual void visit_impl( class HotSpot * _node ) = 0;
          virtual void visit_impl( class TextField * _node ) = 0;
          virtual void visit_impl( class SoundEmitter * _node ) = 0;
          virtual void visit_impl( class Emitter * _node ) = 0;
          virtual void visit_impl( class Point * _node ) = 0;
          virtual void visit_impl( class RigidBody2D * _node ) = 0;
          virtual void visit_impl( class TileMap * _node ) = 0;
          virtual void visit_impl( class Camera2D * _node ) = 0;
          virtual void visit_impl( class Entity * _node ) = 0;
          virtual void visit_impl( class LayerScene * _node ) = 0;		
	};
}
