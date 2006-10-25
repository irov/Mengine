#	include "VisitorBase.h"
#	include "Node.h"
#	include "Allocator.h"
#	include "Renderable.h"
#	include "Layer.h"

using namespace Menge;
using namespace Visitor;
//////////////////////////////////////////////////////////////////////////
void Base::provideUnknown( Node *_node)
{
	_node->visitChildren(this);
}
//////////////////////////////////////////////////////////////////////////
void Base::execute(Node *obj)
{
	provideUnknown(obj);
}
//////////////////////////////////////////////////////////////////////////
void Base::execute(Allocator *obj)
{
	provideUnknown(obj);
}
//////////////////////////////////////////////////////////////////////////
void Base::execute(Renderable *obj)
{
	provideUnknown(obj);
}
//////////////////////////////////////////////////////////////////////////
void Base::execute(Layer *obj)
{
	provideUnknown(obj);
}
//////////////////////////////////////////////////////////////////////////
void Base::operator () (Node *obj)
{
	apply(obj);
}
//////////////////////////////////////////////////////////////////////////
void Base::apply(Node *obj)
{
	obj->visit(this);
}