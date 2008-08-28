#	pragma once

#	include "Node.h"

#	include "InputHandler.h"
#	include "MousePickerTrap.h"

#	include "Math/polygon.h"
#	include "Node.h"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class HotSpot
		: public Node
		, public GlobalMouseHandler
		, public GlobalKeyHandler
		, public MousePickerTrap
	{
		OBJECT_DECLARE(HotSpot)
	
	public:
		HotSpot();
		~HotSpot();

	public:
		const mt::polygon & getPolygon() const;

	protected:
		bool _pickerActive() override;

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

		bool isPicked() const override;
	protected:
		bool _activate() override;
		void _deactivate() override;
		void _render( unsigned int _debugMask ) override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _setListener() override;

	protected:
		mt::polygon m_polygon;
		mt::vec2f m_scale;

	private:		
		bool m_globalMouseEventListener;
		bool m_globalKeyEventListener;

		bool m_onLeaveEvent;
		bool m_onEnterEvent;

		bool m_picked;

#	ifndef MENGE_MASTER_RELEASE
		unsigned long m_debugColor;
#	endif
	};
}