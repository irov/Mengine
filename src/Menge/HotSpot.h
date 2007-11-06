#	pragma once

#	include "SceneNode2D.h"

#	include "InputHandler.h"
#	include "MousePickerTrap.h"

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
		, public InputHandler
		, public GlobalMouseHandler
		, public MousePickerTrap
	{
		OBJECT_DECLARE(HotSpot)
	
	public:
		HotSpot();
		~HotSpot();

	public:
		bool pick( float _x, float _y ) override;
		void onLeave() override;
		void onEnter() override;

	public:
		void addPoint( const mt::vec2f & _p );
		void clearPoints();
		bool testPoint( const mt::vec2f & _p );

		void enableGlobalMouseEvent( bool _value );
		void setHotspotListener( PyObject * _handler );
		
	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	public:
		bool handleGlobalMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleGlobalMouseMove( int _x, int _y, int _whell ) override;

	public:
		void loader( TiXmlElement *_xml) override;

	protected:
		void _update( size_t _timing ) override;
		void _debugRender() override;

	private:
		mt::polygon m_polygon;
		
		bool m_globalMouseEventListenerEnable;
	};
}