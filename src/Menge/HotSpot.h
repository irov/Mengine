#	pragma once

#	include "SceneNode2D.h"

#	include "Math/polygon.h"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

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
		void addPoint( const mt::vec2f &p );
		bool testPoint( const mt::vec2f &p );

		void setHandle( bool _handle );
		void setMouseLeftClickEvent( PyObject * _object );

	public:
		void update( float _timing ) override;
		void loader( TiXmlElement *_xml) override;

	protected:
		void _debugRender() override;
			
	private:
		mt::polygon m_poligon;

		PyObject * m_mouseLeftClick;

		bool m_handle;
	};
}