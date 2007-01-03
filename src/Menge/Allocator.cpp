#	include "Allocator.h"
#	include "ObjectImplement.h"

#	include <algorithm>

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Allocator)
//////////////////////////////////////////////////////////////////////////
Allocator::Allocator()
: m_changePivot(0)
, m_parentAllocator(0)
{
	mt::ident_m4(m_localMatrix);
	mt::ident_m4(m_worldMatrix);
}
//////////////////////////////////////////////////////////////////////////
void Allocator::changePivot()
{
	_changePivot();

	struct FChangePivot
	{
		static void apply( Node *_node)
		{
			Allocator * alloc = dynamic_cast<Allocator*>(_node);
			if( alloc )
			{
				alloc->changePivot();
			}
			else
			{
				_node->foreachFunc( &apply );
			}
		}
	};

	foreachFunc( &FChangePivot::apply );
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
const mt::vec3f & Allocator::getWorldPosition()
{
	const mt::mat4f &wm = getWorldMatrix();

	return wm.v3.v3;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec3f & Allocator::getWorldDirection()
{
	const mt::mat4f &wm = getWorldMatrix();

	return wm.v2.v3;
}
//////////////////////////////////////////////////////////////////////////
const mt::mat4f & Allocator::getWorldMatrix()
{
	if( m_parentAllocator == 0 )
	{
		return getLocalMatrix();
	}

	updateMatrix();

	return m_worldMatrix;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec3f & Allocator::getLocalPosition()const
{
	return m_localMatrix.v3.v3;
}
//////////////////////////////////////////////////////////////////////////
mt::vec3f & Allocator::getLocalPosition()
{
	return m_localMatrix.v3.v3;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec3f & Allocator::getLocalDirection()const
{
	return m_localMatrix.v2.v3;
}
//////////////////////////////////////////////////////////////////////////
mt::vec3f & Allocator::getLocalDirection()
{
	return m_localMatrix.v2.v3;
}
//////////////////////////////////////////////////////////////////////////
const mt::mat4f & Allocator::getLocalMatrix()const
{
	return m_localMatrix;
}
//////////////////////////////////////////////////////////////////////////
mt::mat4f & Allocator::getLocalMatrix()
{
	return m_localMatrix;
}
//////////////////////////////////////////////////////////////////////////
void Allocator::setPosition( const mt::vec3f &position )
{
	mt::vec3f & localPosition = getLocalPosition();
	
	localPosition = position;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::setDirection( const mt::vec3f &direction )
{
	// TODO: FIX 3d set direction
	//m_localMatrix.v0.v2 = direction;
	//m_localMatrix.v1.v2 = mt::perp(direction);

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::setWorldMatrix( const mt::mat4f &matrix )
{
	m_worldMatrix = matrix;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::translate( const mt::vec3f &delta )
{
	m_localMatrix.v3.v3 += delta;

	changePivot();
}
//////////////////////////////////////////////////////////////////////////
void Allocator::_updateParent()
{
	for( Node *parent = getParent(); 
		parent != 0; 
		parent = parent->getParent() )
	{
		m_parentAllocator = dynamic_cast<Allocator*>(parent);
		if( m_parentAllocator )
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

	const mt::mat4f & parentMatrix =
		m_parentAllocator->getWorldMatrix();

	mt::mul_m4_m4(m_worldMatrix,m_localMatrix,parentMatrix);

	m_changePivot = false;
}
//////////////////////////////////////////////////////////////////////////
void Allocator::setParentAllocator( Allocator *_alloc)
{
	m_parentAllocator = _alloc;
}
//////////////////////////////////////////////////////////////////////////
Allocator * Allocator::getParentAllocator()
{
	return m_parentAllocator;
}
//////////////////////////////////////////////////////////////////////////
void Allocator::loader( TiXmlElement * _xml )
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
};