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
	template<class Adapter>
	class VisitorAdapter
		: public Visitor
	{
	public:
		void visit_impl( Node * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Sprite * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Animation * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Layer2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Track * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Arrow * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( HotSpot * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( TextField * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( SoundEmitter * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Emitter * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Point * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( RigidBody2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( TileMap * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Camera2D * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( Entity * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
		void visit_impl( LayerScene * _node ) override { static_cast<Adapter*>(this)->visit( _node ); }	
	};
}
