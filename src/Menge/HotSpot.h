#	pragma once

#	include "SceneNode2D.h"

#	include "InputHandler.h"
#	include "MousePickerTrap.h"

//#	include "Math/b2Polygon.h"

#	include <vector>

#	include "Math/convexpoly2.h"

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
		
	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	public:
		bool handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleGlobalMouseMove( float _x, float _y, int _whell ) override;
		bool handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		void loader( XmlElement *_xml) override;

		virtual void setScale( const mt::vec2f& _scale );

	protected:
		void _update( float _timing ) override;
		bool _compile() override;

		void _debugRender() override;
		virtual void _onSetListener();

	protected:
		std::vector<mt::vec2f> m_points;
		mt::TVecConvex m_polygons;
		mt::vec2f m_scale;
	private:		
		bool m_globalMouseEventListener;
		bool m_globalKeyEventListener;
	};
}