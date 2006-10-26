#	pragma once

#	include "VisitorBase.h"
#	include "Allocator.h"

namespace Menge
{
	namespace Visitor
	{
		//////////////////////////////////////////////////////////////////////////
		class ChangePivotChildren
			: public Base
		{
		public:
			void execute(Allocator * _alloc) override
			{
				_alloc->changePivot();
			}
		};
		//////////////////////////////////////////////////////////////////////////
		void changePivotChildren(Node *_node)
		{
			ChangePivotChildren visitor;

			visitor.apply(_node);
		}
	}
}
