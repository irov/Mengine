#	include "VisitorBase.h"
#	include "Node.h"

using namespace Menge;
using namespace Visitor;

void Base::execute(Node *obj)
{
	obj->visitChildren(this);
			//empty
}

void Base::execute(Allocator *obj)
{
			//empty
}

void Base::execute(Renderable *obj)
{
	//empty
}

void Base::execute(Layer *)
{
	//empty
}

void Base::operator () (Node *obj)
{
	obj->visit(this);	
}
