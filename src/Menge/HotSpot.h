#	pragma once

#	include "Kernel/Node.h"

#   include "Interface/MousePickerSystemInterface.h"

#	include "Core/Polygon.h"

namespace Menge
{
	class Arrow;

	class HotSpot
		: public Node
		, public MousePickerTrapInterface
	{
		DECLARE_VISITABLE();

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
		virtual bool testPoint( const mt::vec2f & _point ) const = 0;
		virtual bool testRadius( const mt::vec2f & _point, float _radius ) const = 0;
		virtual bool testPolygon( const mt::vec2f & _point, const Polygon & _polygon ) const = 0;

	protected:
		bool _activate() override;
		void _afterActivate() override;
		void _deactivate() override;

		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		void _localHide( bool _value ) override;
		void _freeze( bool _value ) override;

	protected:
		void activatePicker_();
		void deactivatePicker_();

	protected:
		bool pick( const mt::vec2f& _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, Arrow * _arrow ) override;
		
	protected:
		PickerTrapState * propagatePickerTrapState() const override;
		Scriptable * propagatePickerScriptable() override;

	protected:
		bool onHandleMouseEnter( float _x, float _y ) override;
		void onHandleMouseLeave() override;
		void onHandleMouseOverDestroy() override;

	public:
		bool handleKeyEvent( const InputKeyEvent & _event ) override;

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

		bool m_defaultHandle;
	};
}
