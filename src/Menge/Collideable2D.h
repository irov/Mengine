#	pragma once

#	include "Math/box2.h"
#	include "Math/polygon.h"

class XmlElement;

namespace Menge
{
	class Collideable2D
	{
	public:
		Collideable2D();

	public:
		void addPoint( const mt::vec2f & _point );

	public:		
		const mt::box2f& getBoundingBox();
		const mt::polygon& getPolygon();

	public:
		void loader( XmlElement * _xml );

	private:
		mt::box2f	m_bbox;
		mt::polygon m_polygon;  
	};
}