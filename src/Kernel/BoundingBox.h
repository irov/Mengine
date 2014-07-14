#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	class BoundingBox
	{
	public:
		BoundingBox();

	public:
		inline const mt::box2f & getBoundingBox() const;

	protected:
		inline void invalidateBoundingBox() const;
		inline bool isInvalidateBoundingBox() const;
		
	protected:
		void updateBoundingBox() const;

	protected:
		virtual void _updateBoundingBox( mt::box2f & _boundingBox ) const;

	protected:
		mutable mt::box2f m_boundingBox;
		mutable bool m_invalidateBoundingBox;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void BoundingBox::invalidateBoundingBox() const
	{
		m_invalidateBoundingBox = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool BoundingBox::isInvalidateBoundingBox() const
	{
		return m_invalidateBoundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::box2f & BoundingBox::getBoundingBox() const
	{
		if( this->isInvalidateBoundingBox() == true )
		{
			this->updateBoundingBox();
		}

		return m_boundingBox;
	}
}
