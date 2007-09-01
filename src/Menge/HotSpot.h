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
		HotSpot();
		~HotSpot();

	public:
		void addPoint( const mt::vec2f & _p );
		bool testPoint( const mt::vec2f & _p );

		void setHandle( bool _handle );
		void setMouseLeftClickEvent( Event * _event );

	public:
		void update( float _timing ) override;
		void loader( TiXmlElement *_xml) override;

	protected:
		void _debugRender() override;
			
	private:
		mt::polygon m_poligon;

		Event * _event;

		bool m_handle;
	};
}