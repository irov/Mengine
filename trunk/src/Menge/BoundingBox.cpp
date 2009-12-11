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
	void BoundingBox::updateBoundingBox()
	{
		if( m_invalidateBoundingBox == true )
		{
			m_invalidateBoundingBox = false;

			_updateBoundingBox( m_boundingBox );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::invalidateBoundingBox()
	{
		m_invalidateBoundingBox = true;

		_invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_invalidateBoundingBox()
	{
		//Empty
	}


}
