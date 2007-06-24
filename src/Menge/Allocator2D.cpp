#	include "Allocator2D.h"
#	include "ObjectImplement.h"

#	include "ObjectForeach.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Allocator2D)
//////////////////////////////////////////////////////////////////////////
Allocator2D::Allocator2D()
: m_changePivot(0)
, m_parentAllocator(0)
{
	mt::ident_m3(m_localMatrix);
	mt::ident_m3(m_worldMatrix);
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::changePivot()
{
	_changePivot();

	ObjectForeach<Allocator2D>(
		m_listChildren.begin(), 
		m_listChildren.end(), 
		std::mem_fun( &Allocator2D::changePivot ) 
		);
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::_changePivot()
{
	m_changePivot = true;
}
//////////////////////////////////////////////////////////////////////////
bool Allocator2D::isChangePivot()const
{
	return m_changePivot;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator2D::getWorldPosition()
{
	const mt::mat3f &wm = getWorldMatrix();

	return wm.v2.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator2D::getWorldDirection()
{
	const mt::mat3f &wm = getWorldMatrix();

	return wm.v0.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::mat3f & Allocator2D::getWorldMatrix()
{
	if( m_parentAllocator == 0 )
	{
		return getLocalMatrix();
	}

	updateMatrix();

	return m_worldMatrix;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator2D::getLocalPosition()const
{
	return m_localMatrix.v2.v2;
}
//////////////////////////////////////////////////////////////////////////
mt::vec2f & Allocator2D::getLocalPosition()
{
	return m_localMatrix.v2.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f & Allocator2D::getLocalDirection()const
{
	return m_localMatrix.v0.v2;
}
//////////////////////////////////////////////////////////////////////////
mt::vec2f & Allocator2D::getLocalDirection()
{
	return m_localMatrix.v0.v2;
}
//////////////////////////////////////////////////////////////////////////
const mt::mat3f & Allocator2D::getLocalMatrix()const
{
	return m_localMatrix;
}
//////////////////////////////////////////////////////////////////////////
mt::mat3f & Allocator2D::getLocalMatrix()
{
	return m_localMatrix;
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::setPosition( const mt::vec2f &position )
{
	mt::vec2f & localPosition = getLocalPosition();

	localPosition = position;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::setDirection( const mt::vec2f &direction )
{
	m_localMatrix.v0.v2 = direction;
	m_localMatrix.v1.v2 = mt::perp(direction);

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::translate( const mt::vec2f &delta )
{
	m_localMatrix.v2.v2 += delta;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::_updateParent()
{
	for( Node *parent = getParent(); 
		parent != 0; 
		parent = parent->getParent() )
	{
		Allocator2D * parentAllocator = dynamic_cast<Allocator2D*>(parent);

		setParentAllocator( parentAllocator );

		if( parentAllocator )
		{
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::updateMatrix()
{
	if( m_changePivot == false )
	{
		return;
	}

	if( m_parentAllocator == 0 )
	{
		return;
	}

	if( _updateMatrix() )
	{
		const mt::mat3f & parentMatrix =
			m_parentAllocator->getWorldMatrix();

		mt::mul_m3_m3(m_worldMatrix,m_localMatrix,parentMatrix);
	}

	m_changePivot = false;
}
//////////////////////////////////////////////////////////////////////////
bool Allocator2D::_updateMatrix()
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::setParentAllocator( Allocator2D *_alloc)
{
	m_parentAllocator = _alloc;
	changePivot();
}
//////////////////////////////////////////////////////////////////////////
Allocator2D * Allocator2D::getParentAllocator()
{
	return m_parentAllocator;
}
//////////////////////////////////////////////////////////////////////////
void Allocator2D::loader( TiXmlElement * _xml )
{
	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("Transformation")
		{
			XML_VALUE_ATTRIBUTE("Value", m_localMatrix);

			changePivot();
		}
	}

	NodeImpl::loader(_xml);
}