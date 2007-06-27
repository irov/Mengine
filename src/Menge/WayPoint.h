#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class WayPoint
		: public SceneNode2D
	{
		OBJECT_DECLARE(WayPoint)

	public:
		WayPoint();
		WayPoint(const mt::vec2f& _pos);

	public:
		void _debugRender() override;

	protected:
		void loader( TiXmlElement * _xml ) override;
	};
};