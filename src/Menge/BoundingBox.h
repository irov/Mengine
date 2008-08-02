#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	class BoundingBox
	{
	public:
		BoundingBox();

	public:
		const mt::box2f & getBoundingBox();

		bool isInvalidateBoundingBox() const;
		void invalidateBoundingBox();

	protected:
		virtual void _updateBoundingBox( mt::box2f & _boundingBox );
		virtual void _invalidateBoundingBox();

	protected:
		mt::box2f m_boundingBox;
		bool m_invalidateBoundingBox;
	};
}