#	pragma once

#	include "SceneNode2D.h"

#	include "Math/polygon.h"

namespace Menge
{
	class ScriptObject;

	class HotSpot
		: public SceneNode2D
	{
		OBJECT_DECLARE(HotSpot)
	
	public:
		HotSpot();

	public:
		void addPoint( const mt::vec2f &p );
		bool testPoint( const mt::vec2f &p );

		void setHandle( bool _handle );
		void setMouseLeftClickEvent( ScriptObject * _object );

	public:
		void update( float _timing ) override;
		void loader( TiXmlElement *_xml) override;

	protected:
		void _debugRender() override;
			
	private:
		mt::polygon m_poligon;

		ScriptObject * m_mouseLeftClick;

		bool m_handle;
	};
}