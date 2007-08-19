#	pragma once

#	include "Math/box2.h"
#	include "Math/polygon.h"

class TiXmlElement;

namespace Menge
{
	class Collideable2D
	{
	public:
		Collideable2D();

	public:
		
		const mt::box2f& getBoundingBox();

	public:
		void loader( TiXmlElement * _xml );

	private:
		mt::box2f	m_bbox;
		mt::polygon m_polygon;  
	};
}