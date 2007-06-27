#	pragma once

#	include "SceneNode2D.h"
#	include "math/polygon.h"

namespace Menge
{
	class WayPoint;

	class Region
		: public SceneNode2D
	{
		OBJECT_DECLARE(Region)

	public:
		Region();
		~Region();
		
	protected:
		void loader( TiXmlElement * _xml ) override;
		void _debugRender() override;
	private:
		mt::polygon	m_polygon;
		typedef std::list<WayPoint*> WayPointList;
		WayPointList	m_helperWayPoints;
	};
};