#	include "Allocator.h"
#	include "ObjectImplement.h"

#	include "VisitorForeach.h"

#	include "Utility/map_algorithm.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Allocator);
//////////////////////////////////////////////////////////////////////////
Allocator::Allocator()
: m_changePivot(0)
, m_parentAllocator(0)
{
	mt::ident_m3(m_localMatrix);
}
//////////////////////////////////////////////////////////////////////////
void Allocator::changePivot()
{
	Visitor::foreach(m_listChildren,&Allocator::changePivot);

	_changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::_changePivot()
{
	m_changePivot = true;
}
//////////////////////////////////////////////////////////////////////////
bool Allocator::isChangePivot()const
{
	return m_changePivot;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator::getWorldPosition()
{
	const mt::mat3f &wm = getWorldMatrix();

	return wm.v2.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator::getWorldDirection()
{
	const mt::mat3f &wm = getWorldMatrix();

	return wm.v0.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::mat3f & Allocator::getWorldMatrix()
{
	if( m_parentAllocator == 0 )
	{
		return getLocalMatrix();
	}

	updateMatrix();

	return m_worldMatrix;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator::getLocalPosition()const
{
	return m_localMatrix.v2.v2;
}
//////////////////////////////////////////////////////////////////////////
mt::vec2f & Allocator::getLocalPosition()
{
	return m_localMatrix.v2.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator::getLocalDirection()const
{
	return m_localMatrix.v0.v2;
}
//////////////////////////////////////////////////////////////////////////
mt::vec2f & Allocator::getLocalDirection()
{
	return m_localMatrix.v0.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::mat3f & Allocator::getLocalMatrix()const
{
	return m_localMatrix;
}
//////////////////////////////////////////////////////////////////////////
mt::mat3f & Allocator::getLocalMatrix()
{
	return m_localMatrix;
}
//////////////////////////////////////////////////////////////////////////
void Allocator::setPosition( const mt::vec2f &position )
{
	mt::vec2f & localPosition = getLocalPosition();
	
	localPosition = position;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::setDirection( const mt::vec2f &direction )
{
	m_localMatrix.v0.v2 = direction;
	m_localMatrix.v1.v2 = mt::perp(direction);

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::setWorldMatrix( const mt::mat3f &matrix )
{
	m_worldMatrix = matrix;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::translate( const mt::vec2f &delta )
{
	m_localMatrix.v2.v2 += delta;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::updateParent()
{
	m_parentAllocator = 0;
	
	for(Node *parent = getParent(); 
		parent != 0; 
		parent = parent->getParent())
	{
		m_parentAllocator = dynamic_cast<Allocator*>(parent);

		if( m_parentAllocator != 0 )
		{
			break;
		}
	}
};
//////////////////////////////////////////////////////////////////////////
void Allocator::updateMatrix()
{
	if( m_changePivot == false )
	{
		return;
	}
	
	if( m_parentAllocator == 0 )
	{
		return;
	}

	const mt::mat3f & parentMatrix =
		m_parentAllocator->getWorldMatrix();

	mt::mul_m3_m3(m_worldMatrix,m_localMatrix,parentMatrix);

	m_changePivot = false;
}