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
//#	include "Layer3D.h"
//#	include "SceneNode3D.h"
#	include "LayerScene.h"
//#	include "NodeCore.h"
//#	include "Actor.h"
//#	include "Camera3D.h"
//#	include "CapsuleController.h"
//#	include "DiscreteEntity.h"
//#	include "FFCamera.h"
//#	include "RigidBody3D.h"

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
		VISITOR_PP_SEQ_ENUM( VISITOR_TYPES )
	};

#	undef VISITOR_PP_SEQ_PROC
#	define VISITOR_PP_SEQ_PROC( TYPE )\
	void visit_impl( class TYPE * _node ) override { static_cast<Adapter*>(this)->visit( _node ); _node->visitChildren( this ); }

	template<class Adapter>
	class VisitorNodePostAdapter
		: public Visitor
	{
	public:
		VISITOR_PP_SEQ_ENUM( VISITOR_TYPES )
	};

#	undef VISITOR_PP_SEQ_PROC
#	define VISITOR_PP_SEQ_PROC( TYPE )\
	void visit_impl( class TYPE * _node ) override { _node->visitChildren( this ); static_cast<Adapter*>(this)->visit( _node ); }

	template<class Adapter>
	class VisitorNodePreAdapter
		: public Visitor
	{
	public:
		VISITOR_PP_SEQ_ENUM( VISITOR_TYPES )
	};
}