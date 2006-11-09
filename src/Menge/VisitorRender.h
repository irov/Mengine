#	pragma once 

#	include "Visitor.h"

#	include "VisitorMaskDefine.h"

namespace Menge
{
	class VisitorRender
		: public Visitor
	{
		VISITOR_MASK_DECLARE(Renderable)

	public:
		VisitorRender();
		VisitorRender(Node *_node);

	public:
#	include "VisitorClassDeclare.h"

	protected:
		void mask_call( Renderable *_node);
	};
}