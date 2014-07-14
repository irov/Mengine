#	include "Kernel/BoundingBox.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BoundingBox::BoundingBox()
		: m_invalidateBoundingBox(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::updateBoundingBox() const
	{
		m_invalidateBoundingBox = false;

		this->_updateBoundingBox( m_boundingBox );
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
        (void)_boundingBox;
		//Empty
	}
}
