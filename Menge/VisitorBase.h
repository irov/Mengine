#	pragma once

namespace RvEngine
{
	class Node;
	class Allocator;
	class Renderable;
	class Layer;

	namespace Visitor
	{
		class Base
		{
		public:
			virtual void execute(Node *);
			virtual void execute(Allocator *);
			virtual void execute(Renderable *);
			virtual void execute(Layer *);
			
		public:
			void operator () (Node *obj);
		};
	}
}