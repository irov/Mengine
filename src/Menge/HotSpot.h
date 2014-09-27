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
		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

	public:
		void setOutward( bool _value );
		bool getOutward() const;

	public:
		bool isMousePickerOver() const;

	public:
		MousePickerTrapInterface * getPickerTrap() override;

	public:
		void setDefaultHandle( bool _handle );
		bool getDefaultHandle() const;

	public:
		PickerTrapState * getPicker() const override;
		    
	public:
		virtual bool testPoint( const mt::vec2f & _point ) const;
		virtual bool testRadius( const mt::vec2f & _point, float _radius ) const;
		virtual bool testPolygon( const mt::vec2f & _point, const Polygon & _polygon ) const;

	public:
		void clearPoints();

	protected:
		bool _activate() override;
		void _afterActivate() override;
		void _deactivate() override;

		//void _invalidateBoundingBox() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
		void _setEventListener( PyObject * _listener ) override;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void _invalidHierarchy() override;

	protected:
		void invalidatePolygonWM();
		void updatePolygonWM_() const;

	public:
		inline const Polygon & getPolygonWM() const;

	protected:
		void _localHide( bool _value ) override;
		void _freeze( bool _value ) override;

	protected:
		void activatePicker_();
		void deactivatePicker_();

	protected:
		bool pick( const mt::vec2f& _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, Arrow * _arrow ) override;
		
	protected:
		PyObject * getPickerEmbed() override;

	protected:
		bool onMouseEnter( const mt::vec2f & _point ) override;
		void onMouseLeave() override;

	protected:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating ) override;

		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;		
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y ) override;
		bool handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel ) override;

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;

	protected:        
		Polygon m_polygon;
		mutable Polygon m_polygonWM;
		mutable Polygon m_polygonWMVM;

		uint32_t m_debugColor;

		bool m_outward;
		
		mutable bool m_invalidatePolygonWM;

	protected:
		PickerTrapState * m_picker;

		bool m_defaultHandle;

	private:
		bool m_onMouseEnterEvent;
		bool m_onMouseLeaveEvent;
		bool m_onMouseMoveEvent;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const Polygon & HotSpot::getPolygonWM() const
	{
		if( m_invalidatePolygonWM == true )
		{
			this->updatePolygonWM_();
		}

		return m_polygonWM;
	}
}
