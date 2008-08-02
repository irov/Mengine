#	include "BoundingBox.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BoundingBox::BoundingBox()
		: m_invalidateBoundingBox(true)
	{
		mt::reset( m_boundingBox, 0.f, 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f & BoundingBox::getBoundingBox()
	{
		if( isInvalidateBoundingBox() == true )
		{
			_updateBoundingBox( m_boundingBox );

			m_invalidateBoundingBox = false;
		}

		return m_boundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BoundingBox::isInvalidateBoundingBox() const
	{
		return m_invalidateBoundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::invalidateBoundingBox()
	{
		m_invalidateBoundingBox = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_invalidateBoundingBox() 
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		//Empty
	}
}