#	include "Allocator2D.h"
#	include "ObjectImplement.h"

#	include <algorithm>

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

template<class T, class I, class F>
inline void dynamic_ptr_for_each( I _begin, I _end, F _f )
{
	struct helper_cast
		: public std::unary_function<T *, void>
	{
		inline helper_cast( F _f )
			: f( _f )
		{
		}

		inline void operator ()( Node * object )
		{
			if( T* t = dynamic_cast<T*>(object) )
			{
				f(t);
			}
			else
			{
				for( Node *it = object->beginChildren(); 
					it != 0;
					it = object->nextChildren()
					)
				{
					operator () ( it );
				}
			}
		}

		F f;
	};

	std::for_each( _begin, _end, helper_cast(_f) );
}

//////////////////////////////////////////////////////////////////////////
void Allocator2D::changePivot()
{
	_changePivot();

	dynamic_ptr_for_each<Allocator2D>(
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
void Allocator2D::setWorldMatrix( const mt::mat3f &matrix )
{
	m_worldMatrix = matrix;

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
		m_parentAllocator = dynamic_cast<Allocator2D*>(parent);
		if( m_parentAllocator )
		{
			break;
		}
	}
};
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
};