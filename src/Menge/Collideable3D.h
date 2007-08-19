#	pragma once

#	include "Math/box3.h"

class TiXmlElement;

namespace Menge
{
	class Collideable3D
	{
	public:
		Collideable3D();

	public:
		
		const mt::box3f& getBoundingBox();

	public:
		void loader( TiXmlElement * _xml );

	private:
		mt::box3f	m_bbox;
	};
}