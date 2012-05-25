#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	class BoundingBox
	{
	public:
		BoundingBox();

	public:
		inline const mt::box2f & getLocalBoundingBox();
		inline bool isInvalidateBoundingBox() const;
	
	protected:
		void invalidateBoundingBox() const;
		virtual void _invalidateBoundingBox();

	protected:
		void updateBoundingBox();

	protected:
		virtual void _updateBoundingBox( mt::box2f & _boundingBox );

	protected:
		mt::box2f m_localBoundingBox;
		mutable bool m_invalidateBoundingBox;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool BoundingBox::isInvalidateBoundingBox() const
	{
		return m_invalidateBoundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::box2f & BoundingBox::getLocalBoundingBox()
	{
		if( m_invalidateBoundingBox == true )
		{
			this->updateBoundingBox();
		}

		return m_localBoundingBox;
	}
}
