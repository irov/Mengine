#	pragma once

#	include "Allocator2D.h"
#	include "math/vec2.h"

namespace Menge
{
	class WayPoint
		: public Allocator2D
	{
		OBJECT_DECLARE(WayPoint)

	public:
		WayPoint();
		WayPoint(const mt::vec2f& _pos);
		void _debugRender() override;

	protected:
		void loader( TiXmlElement * _xml ) override;
	};
};