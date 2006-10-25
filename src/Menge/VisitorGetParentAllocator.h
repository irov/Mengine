#	pragma once

#	include "VisitorBase.h"
#	include "Node.h"

namespace Menge
{
	class Allocator;

	namespace Visitor
	{
		//////////////////////////////////////////////////////////////////////////
		class GetParentAllocator
			: public Base
		{
		public:
			void provideUnknown(Node *_node) override
			{
				Node *parent = _node->getParent();

				if( parent )
				{
					apply(parent);
				}
			}

			void execute(Allocator * _alloc) override
			{
				m_result = _alloc;
			}
		
		public:

			Allocator * getResult()
			{
				return m_result;
			}

		private:
			Allocator * m_result;
		};
		//////////////////////////////////////////////////////////////////////////
		Allocator * getParentAllocator(Node *_node)
		{
			GetParentAllocator visitor;

			visitor.apply(_node);

			Allocator *parentAllocator = visitor.getResult();

			return parentAllocator;
		}
	}
}
