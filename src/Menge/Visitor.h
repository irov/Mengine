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
		void operator() ( Node * _node );

	protected:
		void apply( Node * _node);
	};
}
