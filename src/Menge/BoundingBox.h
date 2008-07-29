#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	class BoundingBox
	{
	public:
		BoundingBox();

	public:
		const mt::box2f & getLocalBoundingBox() const;
		void setLocalBoundingBox( const mt::box2f & _box );

		const mt::box2f & getWorldBoundingBox() const;		

		bool isChangeBoundingBox() const;
		void changeBoundingBox();

		void mergeBoundingBox( const mt::box2f & _bbox );
		void clearWorldBoundingBox();
		void commitWorldBoundingBox();

		void updateBoundingBox();

	protected:
		virtual void _updateBoundingBox( mt::box2f & _localBoundingBox );
		virtual void _changeBoundingBox();

	protected:
		mt::box2f m_localBoundingBox;
		mt::box2f m_worldBoundingBox;
		bool m_changeBoundingBox;
	};
}