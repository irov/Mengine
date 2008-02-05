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
		, public GlobalKeyHandler
		, public MousePickerTrap
	{
		OBJECT_DECLARE(HotSpot)
	
	public:
		HotSpot();
		~HotSpot();

	public:
		bool pick( HotSpot * _hotspot ) override;
		void onLeave() override;
		void onEnter() override;

	public:
		void addPoint( const mt::vec2f & _p );
		bool testPoint( const mt::vec2f & _p );
		void clearPoints();

		void enableGlobalMouseEvent( bool _value );
		void enableGlobalKeyEvent( bool _value );

		void setHotspotListener( PyObject * _handler );
		
	public:
		bool handleKeyEvent( size_t _key, size_t _char, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	public:
		bool handleGlobalMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleGlobalMouseMove( int _x, int _y, int _whell ) override;

	public:
		void loader( XmlElement *_xml) override;

		virtual void setScale( const mt::vec2f& _scale );

	protected:
		void _update( float _timing ) override;
		void _debugRender() override;

	protected:
		mt::polygon m_polygon;
		mt::vec2f m_scale;

	private:		
		bool m_globalMouseEventListener;
		bool m_globalKeyEventListener;
	};
}