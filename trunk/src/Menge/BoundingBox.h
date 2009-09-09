#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	class BoundingBox
	{
	public:
		BoundingBox();

	public:
		inline bool isInvalidateBoundingBox() const;
		

	protected:
		virtual void _updateBoundingBox( mt::box2f & _boundingBox );

	protected:
		mt::box2f m_boundingBox;
		bool m_invalidateBoundingBox;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool BoundingBox::isInvalidateBoundingBox() const
	{
		return m_invalidateBoundingBox;
	}
}
