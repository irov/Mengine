#	pragma once

#	include "Visitor.h"

#	include "Node.h"
#	include "Sprite.h"
#	include "Animation.h"
#	include "Layer2D.h"
#	include "Track.h"
#	include "Arrow.h"
#	include "HotSpot.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "Emitter.h"
#	include "Point.h"
#	include "RigidBody2D.h"
#	include "TileMap.h"
#	include "Camera2D.h"
#	include "Entity.h"
#	include "LayerScene.h"

namespace Menge
{
#	undef VISITOR_PP_SEQ_PROC
#	define VISITOR_PP_SEQ_PROC( TYPE )\
	void visit_impl( class TYPE * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }

	template<class Adapter>
	class VisitorAdapter
		: public Visitor
	{
	public:
		//VISITOR_PP_SEQ_ENUM( VISITOR_TYPES )
		void visit_impl( class Node * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Sprite * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Animation * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Layer2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Track * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Arrow * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class HotSpot * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class TextField * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class SoundEmitter * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Emitter * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Point * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class RigidBody2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class TileMap * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Camera2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Entity * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class LayerScene * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
	};

#	undef VISITOR_PP_SEQ_PROC
#	define VISITOR_PP_SEQ_PROC( TYPE )\
	void visit_impl( class TYPE * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }

	template<class Adapter>
	class VisitorNodePostAdapter
		: public Visitor
	{
	public:
		//VISITOR_PP_SEQ_ENUM( VISITOR_TYPES )
		void visit_impl( class Node * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Sprite * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Animation * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Layer2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Track * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Arrow * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class HotSpot * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class TextField * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class SoundEmitter * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Emitter * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Point * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class RigidBody2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class TileMap * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Camera2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class Entity * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
		void visit_impl( class LayerScene * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }	
	};

#	undef VISITOR_PP_SEQ_PROC
#	define VISITOR_PP_SEQ_PROC( TYPE )\
	void visit_impl( class TYPE * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }

	template<class Adapter>
	class VisitorNodePreAdapter
		: public Visitor
	{
	public:
		//VISITOR_PP_SEQ_ENUM( VISITOR_TYPES )
		void visit_impl( class Node * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }
		void visit_impl( class Sprite * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Animation * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }
		void visit_impl( class Layer2D * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Track * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Arrow * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class HotSpot * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class TextField * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class SoundEmitter * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Emitter * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class Point * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class RigidBody2D * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( class TileMap * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }
		void visit_impl( class Camera2D * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }
		void visit_impl( class Entity * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }
		void visit_impl( class LayerScene * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }	
	};
}