#	include "Kernel/BoundingBox.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BoundingBox::BoundingBox()
		: m_invalidateBoundingBox(true)
	{
		mt::reset( m_localBoundingBox, 0.f, 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::updateBoundingBox()
	{
		m_invalidateBoundingBox = false;

		this->_updateBoundingBox( m_localBoundingBox );
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_updateBoundingBox( mt::box2f & _boundingBox )
	{
        (void)_boundingBox;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::invalidateBoundingBox() const
	{
		m_invalidateBoundingBox = true;

		//this->_invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_invalidateBoundingBox()
	{
		//Empty
	}


}
