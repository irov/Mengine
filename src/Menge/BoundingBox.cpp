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
	void BoundingBox::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		//Empty
	}
}
