#	pragma once

#	include "VisitorBaseDefine.h"

namespace Menge
{
	class Visitor
	{
	public:
		Visitor();

	public:
#	include "VisitorClassDeclare.h"
	public:
		virtual void apply( Node * _node);
	};
}
