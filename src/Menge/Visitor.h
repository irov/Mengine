#	pragma once

namespace Menge
{
	class Node;
	class Allocator;
	class Renderable;
	class HotSpot;
	class Sprite;
	class Dialog;

	class Visitor
	{
	public:
		Visitor();

	public:
		virtual void call( Node *_node, Node *_base);
		virtual void call( Allocator *_node, Node *_base);
		virtual void call( Renderable *_node, Node *_base);
		virtual void call( HotSpot *_node, Node *_base);
		virtual void call( Sprite *_node, Node *_base);
		virtual void call( Dialog *_node, Node *_base);

	public:
		void apply( Node * _node);
	};
}
