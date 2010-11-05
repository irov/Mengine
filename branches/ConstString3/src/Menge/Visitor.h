#	pragma once

namespace Menge
{
	class Visitor
	{
	public:
		virtual void visit_impl( class Node * _node ) = 0;
		virtual void visit_impl( class Sprite * _node ) = 0;
		virtual void visit_impl( class Animation * _node ) = 0;
		virtual void visit_impl( class Layer2D * _node ) = 0;
		virtual void visit_impl( class Track * _node ) = 0;
		virtual void visit_impl( class Arrow * _node ) = 0;
		virtual void visit_impl( class HotSpot * _node ) = 0;
		virtual void visit_impl( class TextField * _node ) = 0;
		virtual void visit_impl( class SoundEmitter * _node ) = 0;
		virtual void visit_impl( class ParticleEmitter * _node ) = 0;
		virtual void visit_impl( class Point * _node ) = 0;
		virtual void visit_impl( class RigidBody2D * _node ) = 0;
		//virtual void visit_impl( class TileMap * _node ) = 0;
		virtual void visit_impl( class Camera2D * _node ) = 0;
		virtual void visit_impl( class Entity * _node ) = 0;
		//virtual void visit_impl( class LayerScene * _node ) = 0;		
	};
}
