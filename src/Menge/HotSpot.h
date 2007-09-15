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
		bool testPoint( const mt::vec2f & _p );

		void setInputHandler( PyObject * _handler );
		
	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, float _whell ) override;

	public:		
		void loader( TiXmlElement *_xml) override;

	protected:
		bool _activate() override;
		void _deactivate() override;
		void _update( float _timing ) override;
		void _debugRender() override;

	private:
		mt::polygon m_poligon;

		PyObject * m_handler;

		//bool m_handle;

		bool m_onLeaveMethod;
		bool m_onEnterMethod;

		bool onHandleMouseButtonEvent;
	};
}