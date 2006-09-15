#	include "VisitorUpdateWorldMatrix.h"

#	include "ObjectAllocator.h"

using namespace RvEngine;
using namespace Visitor;
using namespace Object;

void Base::execute(Node *obj)
{
	Node *parent = obj->getParent();

	parent->visit(this);
			//empty
}

void Base::execute(Object::Allocator *obj)
{
	const mt::mat3f &wm = obj->getWorldMatrix();

	//if( m_parent && m_changePivot )
	//{	
	//	const mt::mat3f & parentMatrix =
	//		m_parent->getWorldMatrix();

	//	mt::Mul(m_worldMatrix,m_localMatrix,parentMatrix);

	//	m_changePivot = false;
	//}
}