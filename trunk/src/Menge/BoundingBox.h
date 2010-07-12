#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	class BoundingBox
	{
	public:
		BoundingBox();

	public:
		inline const mt::box2f & getBoundingBox();
		inline bool isInvalidateBoundingBox() const;
	
	protected:
		virtual void invalidateBoundingBox();
		virtual void _invalidateBoundingBox();

	protected:
		virtual void updateBoundingBox();
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
	//////////////////////////////////////////////////////////////////////////
	inline const mt::box2f & BoundingBox::getBoundingBox()
	{
		if( m_invalidateBoundingBox == true )
		{
			updateBoundingBox();
		}

		return m_boundingBox;
	}

}
