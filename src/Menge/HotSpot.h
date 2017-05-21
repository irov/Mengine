#	pragma once

#	include "Kernel/Node.h"

#   include "Interface/MousePickerSystemInterface.h"

#	include "Core/Polygon.h"

namespace Menge
{
	class Arrow;

    //////////////////////////////////////////////////////////////////////////
    enum HotSpotEventFlag
    {
        EVENT_KEY = 0,
		EVENT_TEXT,
        EVENT_MOUSE_BUTTON,
        EVENT_MOUSE_BUTTON_BEGIN,
        EVENT_MOUSE_BUTTON_END,
        EVENT_MOUSE_WHEEL,
        EVENT_MOUSE_MOVE,
        EVENT_MOUSE_ENTER,
        EVENT_MOUSE_LEAVE,
        EVENT_MOUSE_OVER_DESTROY,
        EVENT_ACTIVATE,
        EVENT_DEACTIVATE
    };
    //////////////////////////////////////////////////////////////////////////
    class HotSpotEventReceiver
        : public EventReceiver
    {
    public:
        virtual void onHotSpotActivate() = 0;
        virtual void onHotSpotDeactivate() = 0;
        virtual void onHotSpotMouseLeave() = 0;
        virtual bool onHotSpotMouseEnter( float _x, float _y ) = 0;
        virtual bool onHotSpotKey( const InputKeyEvent & _event ) = 0;
		virtual bool onHotSpotText( const InputTextEvent & _event ) = 0;
        virtual bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseMove( const InputMouseMoveEvent & _event ) = 0;
        virtual bool onHotSpotMouseWheel( const InputMouseWheelEvent & _event ) = 0;
        virtual void onHotSpotMouseOverDestroy() = 0;
        virtual void onHotSpotMouseButtonBegin( uint32_t _enumerator, bool _isEnd ) = 0;
        virtual void onHotSpotMouseButtonEnd( uint32_t _enumerator, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class HotSpot
		: public Node
		, public Eventable
		, public MousePickerTrapInterface
	{
		DECLARE_VISITABLE( Node );
        EVENT_RECEIVER( HotSpotEventReceiver );

	public:
		HotSpot();
		~HotSpot();

	public:
		void setOutward( bool _value );
		bool getOutward() const;

	public:
		void setGlobal( bool _value );
		bool getGlobal() const;

	public:
		bool isMousePickerOver() const;

	public:
		MousePickerTrapInterface * getPickerTrap() override;

	public:
		void setDefaultHandle( bool _handle );
		bool getDefaultHandle() const;
		    
	public:
		virtual bool testPoint( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const = 0;
		virtual bool testRadius( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const = 0;
		virtual bool testPolygon( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const = 0;

	protected:
		bool _activate() override;
		void _afterActivate() override;
		void _deactivate() override;

	protected:
		void _localHide( bool _value ) override;
		void _freeze( bool _value ) override;

	protected:
		void _invalidateWorldMatrix() override;

	protected:
		void activatePicker_();
		void deactivatePicker_();

	protected:
		bool pick( const mt::vec2f& _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const Resolution & _contentResolution, Arrow * _arrow ) override;
		
	protected:
		PickerTrapState * propagatePickerTrapState() const override;
		Scriptable * propagatePickerScriptable() override;

	protected:
		bool onHandleMouseEnter( float _x, float _y ) override;
		void onHandleMouseLeave() override;
		void onHandleMouseOverDestroy() override;

	public:
		bool handleKeyEvent( const InputKeyEvent & _event ) override;
		bool handleTextEvent( const InputTextEvent & _event ) override;

	public:
		bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
		bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
		bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

	protected:        
		uint32_t m_debugColor;

		bool m_outward;	
		bool m_global;

	protected:
		PickerTrapState * m_picker;

		//typedef std::vector<pybind::object> TVectorHandleMouseEnter

		bool m_defaultHandle;
	};
}
