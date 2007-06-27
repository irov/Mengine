#	pragma once

#	include "SceneNode2D.h"

#	include "Math/polygon.h"

namespace Menge
{
	class HotSpot
		: public SceneNode2D
	{
		OBJECT_DECLARE(HotSpot)
	
	public:
		void addPoint( const mt::vec2f &p );
		bool testPoint( const mt::vec2f &p );

	public:
		void loader( TiXmlElement *_xml) override;

	protected:
		void _debugRender() override;
			
	private:
		mt::polygon m_poligon;
	};
}